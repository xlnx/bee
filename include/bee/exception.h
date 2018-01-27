#ifndef BEE_EXCEPTION_H
#define BEE_EXCEPTION_H

#ifdef WIN32
#include <windows.h>
#endif
#include <string>
#include <sstream>
#include <exception>
#ifdef WIN32
#	include <windows.h>
#endif
#include "stackTrace.h"
#include "runtime.h"

namespace bee
{

class Runtime;

namespace exception
{

#ifdef WIN32

LONG CALLBACK veh(PEXCEPTION_POINTERS ep);

#endif

struct Fatal: std::exception
{
	Fatal(const std::string &errloc, std::string content = "");
	const char *what() const noexcept override
	{
		return errmsg.c_str();
	}
protected:
	std::string errmsg;
};

struct ScriptError: Fatal
{
	ScriptError(const std::string &errloc):
		Fatal(errloc)
	{}
};

struct ResourceError: Fatal
{
	ResourceError(const std::string &errloc):
		Fatal(errloc)
	{}
};

}

}

#endif