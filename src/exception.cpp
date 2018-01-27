#include "bee/exception.h"

namespace bee
{

namespace exception
{

#ifdef WIN32

#define BEE_VEH_CODE 0x0f728f38 // random generated code, pls don't collide XD

static LPCONTEXT contextPtr = nullptr;

LONG CALLBACK veh(PEXCEPTION_POINTERS ep)
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
	runtime.traceBack = ss.str();
}

}

}