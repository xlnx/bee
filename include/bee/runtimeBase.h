#pragma once

#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>

namespace bee
{

#define BEE_STRINGLIFY_(X) #X

#define BEE_STRINGLIFY(X) BEE_STRINGLIFY_(X)

#define BEE_LOG(...) ::bee::RuntimeBase::getInstance().log(\
	::bee::shortenFileName(__FILE__), \
	":" BEE_STRINGLIFY(__LINE__) "::", __func__, "(): ", ##__VA_ARGS__)

#define BEE_RAISE(Type, ...) throw ::bee::exception::Type(\
	::std::string("<" BEE_STRINGLIFY(Type) "> ") + ::bee::shortenFileName(__FILE__) + \
	":" BEE_STRINGLIFY(__LINE__) "::" + __func__ + "()", ##__VA_ARGS__);

#define BEE_EXCEPTION_BEGIN try {

#define BEE_EXCEPTION_END }\
	catch (::bee::exception::Fatal &e)\
	{\
		log(e.what());\
		coredump();\
	}\
	catch (::std::exception &e)\
	{\
		log("This application has been trapped in an unexpected condition.");\
		coredump();\
	}

namespace exception
{

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

}

class RuntimeBase
{
	friend class exception::Fatal;
public:
	RuntimeBase();
	virtual ~RuntimeBase();

	template <typename ... Types>
		void log(Types... messages) noexcept
		{
			auto s = genTimeStamp();
			std::cerr << "[" << s << "] "; dumpWriter << "[" << s << "] ";
			logUtil(messages...);
			std::cerr << std::endl; dumpWriter << std::endl;
		}
	void dump(std::string logFileName = "") noexcept;
	void coredump(std::string logFileName = "") noexcept;
private:
	std::string genTimeStamp() noexcept
	{
		time_t timep; time(&timep); char tmp[128];
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H-%M-%S", localtime(&timep));
		return tmp;
	}
	template <typename T, typename ... Types>
		void logUtil(T &&message, Types... other) noexcept
		{
			std::cerr << std::forward<T>(message); dumpWriter << std::forward<T>(message);
			logUtil(other...);
		}
	template <typename T>
		void logUtil(T &&message) noexcept
		{
			std::cerr << std::forward<T>(message); dumpWriter << std::forward<T>(message);
		}
	std::ofstream getDumpStream(const std::string &logFileName);
public:
	static RuntimeBase &getInstance()
	{
		return *instance;
	}
private:
	std::ostringstream dumpWriter;
	std::string traceBack;
	static RuntimeBase *instance;
};

constexpr inline const char *shortenFileName(const char str[])
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