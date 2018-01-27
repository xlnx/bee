#include "bee/exception.h"

namespace bee
{

namespace exception
{

Fatal::Fatal(const std::string &errloc, std::string content):
	errmsg(errloc + "\t" + content)
{
#	ifdef WIN32
	CONTEXT context;
	GetThreadContext(GetCurrentThread(), &context);
#	endif
	bee::stackTrace(stackWriter, BEE_EXCEPTION_TRACE_DEPTH
#	ifdef WIN32
		, &context			
#	endif
	);
}

}

}