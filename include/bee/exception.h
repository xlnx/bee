#pragma once

#include <exception>
#include <string>

namespace bee
{

struct Fatal: std::exception
{
	Fatal(const std::string &str):
		content(str)
	{}
	Fatal(std::string &&str):
		content(str)
	{}
	const char *what() const noexcept override
	{
		return content.c_str();
	}
protected:
	std::string content;
};

struct ScriptError: Fatal
{

};

struct ResourceError: Fatal
{

};

}