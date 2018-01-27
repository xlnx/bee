#include "bee/runtimeBase.h"
#include "bee/stackTrace.h"
#ifdef WIN32
#	include <windows.h>
#	include <io.h>
#	include <direct.h>
#else
#	include <unistd.h>
#endif

namespace bee
{

RuntimeBase *RuntimeBase::instance = nullptr;

// RuntimeBase Startup

#ifdef WIN32

static const char *errorLookup[0x40];

static LPCONTEXT exceptionContext = nullptr;

static LONG WINAPI handleException(LPEXCEPTION_POINTERS info)
{
	BEE_LOG("<", errorLookup[info->ExceptionRecord->ExceptionCode & 0x3F], "> Exception code:",
		std::hex, info->ExceptionRecord->ExceptionCode, std::dec);
	bee::exceptionContext = info->ContextRecord;
	RuntimeBase::getInstance().coredump();
	return EXCEPTION_CONTINUE_SEARCH;
}

#endif

namespace exception
{

#ifdef WIN32

#define BEE_VEH_CODE 0x0f728f38 // random generated code, pls don't collide XD

static LPCONTEXT contextPtr = nullptr;

static LONG CALLBACK veh(PEXCEPTION_POINTERS ep)
{
	if (ep->ExceptionRecord->ExceptionCode == BEE_VEH_CODE)
	{
		contextPtr = new CONTEXT(*ep->ContextRecord);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

#endif

Fatal::Fatal(const std::string &errloc, std::string content):
	errmsg(errloc + "\t" + content)
{
#	ifdef WIN32
	RaiseException(BEE_VEH_CODE, 0, 0, nullptr);
#	endif
	std::ostringstream ss;
	bee::stackTrace(ss, BEE_EXCEPTION_TRACE_DEPTH
#	ifdef WIN32
		, contextPtr
#	endif
	);
	delete contextPtr;
	contextPtr = nullptr;
	RuntimeBase::instance->traceBack = ss.str();
}

}

RuntimeBase::RuntimeBase()
{
#	ifdef WIN32
	errorLookup[EXCEPTION_ACCESS_VIOLATION & 0x3F] = 
		"Access violation";
	errorLookup[EXCEPTION_ARRAY_BOUNDS_EXCEEDED & 0x3F] = 
		"Array bounds exceeded";
	errorLookup[EXCEPTION_BREAKPOINT & 0x3F] = 
		"Breakpoint";
	errorLookup[EXCEPTION_DATATYPE_MISALIGNMENT & 0x3F] = 
		"Datatype misalignment";
	errorLookup[EXCEPTION_FLT_DENORMAL_OPERAND & 0x3F] = 
		"Float denormal operand";
	errorLookup[EXCEPTION_FLT_DIVIDE_BY_ZERO & 0x3F] = 
		"Float divide by zero";
	errorLookup[EXCEPTION_FLT_INEXACT_RESULT & 0x3F] = 
		"Float inexact result";
	errorLookup[EXCEPTION_FLT_INVALID_OPERATION & 0x3F] = 
		"FLoat invalid operation";
	errorLookup[EXCEPTION_FLT_OVERFLOW & 0x3F] = 
		"Float overflow";
	errorLookup[EXCEPTION_FLT_STACK_CHECK & 0x3F] = 
		"Float stack check";
	errorLookup[EXCEPTION_FLT_UNDERFLOW & 0x3F] = 
		"Float underflow";
	errorLookup[EXCEPTION_ILLEGAL_INSTRUCTION & 0x3F] = 
		"Illegal instruction";
	errorLookup[EXCEPTION_IN_PAGE_ERROR & 0x3F] = 
		"In page error";
	errorLookup[EXCEPTION_INT_DIVIDE_BY_ZERO & 0x3F] = 
		"Integer divide by zero";
	errorLookup[EXCEPTION_INT_OVERFLOW & 0x3F] = 
		"Integer overflow";
	errorLookup[EXCEPTION_INVALID_DISPOSITION & 0x3F] = 
		"Invalid disposition";
	errorLookup[EXCEPTION_NONCONTINUABLE_EXCEPTION & 0x3F] = 
		"Noncontinuable exception";
	errorLookup[EXCEPTION_PRIV_INSTRUCTION & 0x3F] = 
		"Privilege instruction";
	errorLookup[EXCEPTION_SINGLE_STEP & 0x3F] = 
		"Single step";
	errorLookup[EXCEPTION_STACK_OVERFLOW & 0x3F] = 
		"Stack overflow";
	AddVectoredExceptionHandler(1, exception::veh);
	SetUnhandledExceptionFilter(handleException);
#	else
#		warn Coredump not fully supported on this platform.
#	endif
	if (RuntimeBase::instance)
	{
		BEE_RAISE(Fatal, "Could not instantiate SINGLETON class 'bee::RuntimeBase'.");
	}
	else
	{
		RuntimeBase::instance = this;
	}
}

RuntimeBase::~RuntimeBase()
{
}

void RuntimeBase::dump(std::string logFileName) noexcept
{
	auto writer(getDumpStream(logFileName));
	writer << dumpWriter.str() << std::endl;
}

void RuntimeBase::coredump(std::string logFileName) noexcept
{
	auto writer(getDumpStream(logFileName));
	writer << "**bee** built " << __DATE__ << " " << __TIME__ << std::endl << std::endl;
	writer << dumpWriter.str() << std::endl;
	if (traceBack != "")
	{
		writer << traceBack;
	}
	else
	{
		stackTrace(writer, BEE_EXCEPTION_TRACE_DEPTH, 
#		ifdef WIN32
			exceptionContext
#		endif
		);
	}
	writer.close();
	std::terminate();
}

std::ofstream RuntimeBase::getDumpStream(const std::string &logFileName)
{
#	ifdef WIN32
	if (access("log/", 02))
	{
		mkdir("log");
	}
#	else
#	endif
	if (logFileName == "")
	{
		return std::ofstream("log/" + genTimeStamp() + ".log");
	}
	return std::ofstream("log/" + logFileName);
}

}