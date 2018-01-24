#pragma once

#include <exception>
#include <string>

namespace bee
{

namespace exception
{

struct Fatal: std::exception
{
	Fatal(const std::string &errloc, std::string content = ""):
		errmsg(errloc + "\t" + content)
	{}
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