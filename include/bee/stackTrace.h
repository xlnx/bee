#pragma once

#include "runtime.h"
#include <iostream>
#ifdef WIN32
#	include <windows.h>
#endif

namespace bee
{

#define BEE_EXCEPTION_TRACE_DEPTH 128

void stackTrace(std::ostream &os, unsigned depth
#ifdef WIN32
	, LPCONTEXT info
#endif
);

}