#ifdef WIN32
#define PACKAGE "stackTrace"
#define PACKAGE_VERSION "0.1"
#include <windows.h> 
#include <excpt.h>
#include <imagehlp.h>
#include <bfd.h>
#include <psapi.h>
#include <cstring>
#endif
#include "bee/stackTrace.h"

namespace bee
{

static std::ostream *out;

#ifdef WIN32

#define BUFFER_MAX (16*1024)

#define BFD_ERR_OK          (0)
#define BFD_ERR_OPEN_FAIL   (1)
#define BFD_ERR_BAD_FORMAT  (2)
#define BFD_ERR_NO_SYMBOLS  (3)
#define BFD_ERR_READ_SYMBOL (4)

static const char *const bfd_errors[] = {
	"",
	"(Failed to open bfd)",
	"(Bad format)",
	"(No symbols)",
	"(Failed to read symbols)",
};

struct bfd_ctx {
	bfd * handle;
	asymbol ** symbol;
	bfd_ctx():
		handle(nullptr), symbol(nullptr)
	{}
};

struct bfd_set {
	char * name;
	bfd_ctx * bc;
	bfd_set *next;
	bfd_set(): 
		name(nullptr), bc(nullptr), next(nullptr)
	{}
};

struct find_info {
	asymbol **symbol;
	bfd_vma counter;
	const char *file;
	const char *func;
	unsigned line;
	find_info():
		symbol(nullptr), counter(0), file(nullptr), func(nullptr), line(0)
	{}
};

static void lookup_section(bfd *abfd, asection *sec, void *opaque_data)
{
	find_info *data = reinterpret_cast<find_info*>(opaque_data);

	if (data->func)
		return;

	if (!(bfd_get_section_flags(abfd, sec) & SEC_ALLOC)) 
		return;

	bfd_vma vma = bfd_get_section_vma(abfd, sec);
	if (data->counter < vma || vma + bfd_get_section_size(sec) <= data->counter) 
		return;

	bfd_find_nearest_line(abfd, sec, data->symbol, data->counter - vma, &(data->file), &(data->func), &(data->line));
}

static void find(bfd_ctx * b, DWORD offset, const char **file, const char **func, unsigned *line)
{
	find_info data;
	data.func = nullptr;
	data.symbol = b->symbol;
	data.counter = offset;
	data.file = nullptr;
	data.func = nullptr;
	data.line = 0;

	bfd_map_over_sections(b->handle, &lookup_section, &data);
	if (file) {
		*file = data.file;
	}
	if (func) {
		*func = data.func;
	}
	if (line) {
		*line = data.line;
	}
}

static int init_bfd_ctx(bfd_ctx *bc, const char * procname, int *err)
{
	bc->handle = nullptr;
	bc->symbol = nullptr;

	bfd *b = bfd_openr(procname, 0);
	if (!b) {
		if(err) { *err = BFD_ERR_OPEN_FAIL; }
		return 1;
	}

	if(!bfd_check_format(b, bfd_object)) {
		bfd_close(b);
		if(err) { *err = BFD_ERR_BAD_FORMAT; }
		return 1;
	}

	if(!(bfd_get_file_flags(b) & HAS_SYMS)) {
		bfd_close(b);
		if(err) { *err = BFD_ERR_NO_SYMBOLS; }
		return 1;
	}

	asymbol **symbol_table;

	unsigned dummy = 0;
	if (bfd_read_minisymbols(b, FALSE, reinterpret_cast<void**>(&symbol_table), &dummy) == 0) {
		if (bfd_read_minisymbols(b, TRUE, reinterpret_cast<void**>(&symbol_table), &dummy) < 0) {
			delete symbol_table;
			bfd_close(b);
			if(err) { *err = BFD_ERR_READ_SYMBOL; }
			return 1;
		}
	}

	bc->handle = b;
	bc->symbol = symbol_table;

	if(err) { *err = BFD_ERR_OK; }
	return 0;
}

static void close_bfd_ctx(bfd_ctx *bc)
{
	if (bc) {
		if (bc->symbol) {
			delete bc->symbol;
		}
		if (bc->handle) {
			bfd_close(bc->handle);
		}
	}
}

static bfd_ctx *get_bc(bfd_set *set , const char *procname, int *err)
{
	while(set->name) {
		if (strcmp(set->name , procname) == 0) {
			return set->bc;
		}
		set = set->next;
	}
	bfd_ctx bc;
	if (init_bfd_ctx(&bc, procname, err)) {
		return nullptr;
	}
	set->next = new bfd_set;
	set->bc = new bfd_ctx;
	memcpy(set->bc, &bc, sizeof(bc));
	set->name = strdup(procname);
	return set->bc;
}

static void release_set(bfd_set *set)
{
	while(set) {
		bfd_set * temp = set->next;
		delete set->name;
		close_bfd_ctx(set->bc);
		delete set;
		set = temp;
	}
}

static void _backtrace(bfd_set *set, int depth , LPCONTEXT context)
{
	char procname[MAX_PATH];
	GetModuleFileNameA(nullptr, procname, sizeof procname);

	bfd_ctx *bc = nullptr;
	int err = BFD_ERR_OK;

	STACKFRAME frame;
	memset(&frame,0,sizeof(frame));

	frame.AddrPC.Offset = context->Rip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context->Rsp;
	frame.AddrStack.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context->Rbp;
	frame.AddrFrame.Mode = AddrModeFlat;

	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	char symbol_buffer[sizeof(IMAGEHLP_SYMBOL) + 255];
	char module_name_raw[MAX_PATH];

	while(StackWalk(
#		if defined(__IA64__)
			IMAGE_FILE_MACHINE_IA64
#		elif defined(__i386__) || defined(__INTEL__) || defined(_M_IX86)
			IMAGE_FILE_MACHINE_I386
#		elif defined(__amd64) || defined(__x86_64__) || defined(_M_X64)
			IMAGE_FILE_MACHINE_AMD64
#		else 
#			warn Unrecognized architecture.
#		endif
		,
		process, 
		thread, 
		&frame, 
		context, 
		0, 
		SymFunctionTableAccess, 
		SymGetModuleBase, 0)) {
		
		--depth;
		if (depth < 0)
			break;
		
		IMAGEHLP_SYMBOL *symbol = (IMAGEHLP_SYMBOL *)symbol_buffer;
		symbol->SizeOfStruct = (sizeof *symbol) + 255;
		symbol->MaxNameLength = 254;
		
		DWORD64 module_base = SymGetModuleBase(process, frame.AddrPC.Offset);
		const char * module_name = "[unknown module]";
		if (module_base && 
			GetModuleFileNameA((HINSTANCE)module_base, module_name_raw, MAX_PATH)) {
			module_name = module_name_raw;
			bc = get_bc(set, module_name, &err);
		}
		
		const char * file = nullptr;
		const char * func = nullptr;
		unsigned line = 0;
		
		if (bc) {
			find(bc,frame.AddrPC.Offset,&file,&func,&line);
		}
		
		if (file == nullptr) {
			DWORD64 dummy = 0;
			if (SymGetSymFromAddr(process, frame.AddrPC.Offset, &dummy, symbol)) {
				file = symbol->Name;
			}
			else {
				file = "[unknown file]";
			}
		}
		if (func == nullptr) {
			*out << "[" << module_name << ":0x" <<
				std::hex << frame.AddrPC.Offset << std::dec << "] " <<
				shortenFileName(file) << " " << 
				bfd_errors[err] << std::endl;
		}
		else {
			*out << "[" << module_name << ":0x" <<
				std::hex << frame.AddrPC.Offset << std::dec << "] " <<
				shortenFileName(file) << ":" << 
				line << "::" << 
				func << "()" << std::endl;
		}
	}
}

#else

#include <execinfo.h>

#endif

void stackTrace(std::ostream &os, unsigned depth
#	ifdef WIN32
		, LPCONTEXT context
#	endif
	)
{
	out = &os;
#	ifdef WIN32
		if (!context)
		{
			*out << "No stack information provided." << std::endl;
		}
		else if (!SymInitialize(GetCurrentProcess(), 0, TRUE))
		{
			*out << "Failed to init symbol context." << std::endl;
		}
		else 
		{
			bfd_init();
			bfd_set *set = new bfd_set;
			_backtrace(set, depth, context);
			release_set(set);
		}
#	else
#		warn stackTrace not implemented.
#	endif
}

}