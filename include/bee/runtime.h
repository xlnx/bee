#pragma once

#include "exception.h"
#include "util.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>

namespace bee
{

#define BEE_STRINGLIFY_(X) #X

#define BEE_STRINGLIFY(X) BEE_STRINGLIFY_(X)

#define BEE_LOG(...) bee::runtime.log(\
	bee::shortenFileName(__FILE__), \
	":" BEE_STRINGLIFY(__LINE__) "::", __func__, "(): ", ##__VA_ARGS__)

#define BEE_RAISE(Type, ...) throw bee::exception::Type(\
	std::string("<" BEE_STRINGLIFY(Type) "> ") + bee::shortenFileName(__FILE__) + \
	":" BEE_STRINGLIFY(__LINE__) "::" + __func__ + "()", ##__VA_ARGS__);

class Runtime final
{
public:
	Runtime();
	~Runtime();

	void createWindow(const std::string &title = "bee~", 
			bool fullscreen = false, 
			unsigned width = 1024, 
			unsigned height = 768);
	void shell(const std::string &script);
	template <typename ... Types>
		void log(Types... messages)
		{
			auto s = genTimeStamp();
			std::cerr << "[" << s << "] ";
			dumpWriter << "[" << s << "] ";
			logUtil(messages...);
			std::cerr << std::endl;
			dumpWriter << std::endl;
		}
	// void log(const std::string &message);
	void dump(std::string logFileName = "");
	void coreDump(std::string logFileName = "");
	// 
	void test();
	void exec(const std::function<void()> &fn);
private:
	void initializeGraphics();
	void finalizeGraphics();
	std::string genTimeStamp();
	template <typename T, typename ... Types>
		void logUtil(T &&message, Types... other)
		{
			std::cerr << std::forward<T>(message);
			dumpWriter << std::forward<T>(message);
			logUtil(other...);
		}
	template <typename T>
		void logUtil(T &&message)
		{
			std::cerr << std::forward<T>(message);
			dumpWriter << std::forward<T>(message);
		}
	void dumpUtil(std::ofstream &of);
public:
	GLFWwindow *window = nullptr;
private:
	std::ostringstream dumpWriter;
	static bool haveInstance;
};

extern Runtime runtime;

constexpr const char *shortenFileName(const char str[])
{
	const char *q = str;
	for (auto p = str; *p != 0; ++p)
	{
		if (*p == '\\' || *p == '/')
		{
			q = p + 1;
		}
	}
	return q;
}

}