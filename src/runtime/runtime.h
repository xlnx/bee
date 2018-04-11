#pragma once

#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>
#include <functional>
#include <stack>
#ifdef WIN32
#	include <windows.h>
#endif

#ifdef BEE_RUNTIME_INTRUSIVE
int main(int argc, char **argv);
#endif

namespace bee
{

#define BEE_STRINGLIFY_(X) #X

#define BEE_STRINGLIFY(X) BEE_STRINGLIFY_(X)

#define BEE_LOG(...) ::bee::Runtime::log(\
	::bee::shortenFileName(__FILE__), \
	":" BEE_STRINGLIFY(__LINE__) "::", __func__, "(): ", ##__VA_ARGS__)

#define BEE_RAISE(Type, ...) throw ::bee::exception::Type(__VA_ARGS__)\
	.addInfo(::std::string("<" BEE_STRINGLIFY(Type) "> ") + ::bee::shortenFileName(__FILE__) + \
	":" BEE_STRINGLIFY(__LINE__) "::" + __func__ + "()")\

namespace exception
{

struct Fatal: std::exception
{
	Fatal(std::string content = "");
	const char *what() const noexcept override
	{
		return errmsg.c_str();
	}
	Fatal &addInfo(const std::string &info)
	{
		errmsg = info + "\t" + errmsg;
		return *this;
	}
protected:
	std::string errmsg;
};

}

class Runtime
{
	friend class exception::Fatal;
#	ifdef WIN32
	friend LONG WINAPI handleException(LPEXCEPTION_POINTERS info);
#	endif
#	ifdef BEE_RUNTIME_INTRUSIVE
	friend int ::main(int argc, char **argv);
#	endif
public:
	Runtime();
	virtual ~Runtime();

	template <typename ... Types>
		static void log(Types... messages) noexcept
		{
			auto s = genTimeStamp();
			std::cerr << "[" << s << "] "; haveInstance->dumpWriter << "[" << s << "] ";
			logUtil(messages...);
			std::cerr << std::endl; haveInstance->dumpWriter << std::endl;
		}
	static void dump(std::string logFileName = "") noexcept;
#	ifndef BEE_RUNTIME_INTRUSIVE
	static void exec(std::function<void()> target) noexcept;
#	endif
	static void onCoredump(std::function<void()> callback);
private:
	static std::string genTimeStamp() noexcept
	{
		time_t timep; time(&timep); char tmp[128];
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H-%M-%S", localtime(&timep));
		return tmp;
	}
	template <typename T, typename ... Types>
		static void logUtil(T &&message, Types... other) noexcept
		{
			std::cerr << std::forward<T>(message); haveInstance->dumpWriter << std::forward<T>(message);
			logUtil(other...);
		}
	template <typename T>
		static void logUtil(T &&message) noexcept
		{
			std::cerr << std::forward<T>(message); haveInstance->dumpWriter << std::forward<T>(message);
		}
	static std::ofstream getDumpStream(const std::string &logFileName);
	static void coredump(std::string logFileName = "") noexcept;
private:
	std::ostringstream dumpWriter;
	std::string traceBack;
#	ifndef BEE_RUNTIME_INTRUSIVE
	static int execDepth;
#	endif
	std::stack<std::function<void()>> dumpCallback;
	static Runtime *haveInstance;
};

constexpr inline const char *shortenFileName(const char str[])
{
	const char *q = str;
	for (auto p = str; *p != 0; ++p)
		if (*p == '\\' || *p == '/')
			q = p + 1;
	return q;
}

}