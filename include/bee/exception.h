#ifndef BEE_EXCEPTION_H
#define BEE_EXCEPTION_H

#ifdef WIN32
#include <windows.h>
#endif
#include <string>
#include <sstream>
#include <exception>
#include "stackTrace.h"
#include "runtime.h"

namespace bee
{

class Runtime;

namespace exception
{

struct Fatal: std::exception
{
	Fatal(const std::string &errloc, std::string content = "");
	const char *what() const noexcept override
	{
		return errmsg.c_str();
	}
	friend class bee::Runtime;
protected:
	std::string errmsg;
	std::ostringstream stackWriter;
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