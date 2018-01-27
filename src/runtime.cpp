#include "bee/runtime.h"
#include "bee/stackTrace.h"
#include "new_parser/parser.h"
#ifdef WIN32
#	include <windows.h>
#	include <io.h>
#	include <direct.h>
#else
#	include <unistd.h>
#endif

namespace bee
{

bool Runtime::haveInstance = false;

Runtime runtime;

#define BEE_EXCEPTION_BEGIN try {

#define BEE_EXCEPTION_END }\
	catch (exception::Fatal &e)\
	{\
		log(e.what());\
		coredump();\
	}\
	catch (std::exception &e)\
	{\
		log("This application has been trapped in an unexpected condition.");\
		coredump();\
	}

// Runtime Startup

#ifdef WIN32

static const char *errorLookup[0x40];

static LPCONTEXT exceptionContext = nullptr;

static LONG WINAPI handleException(LPEXCEPTION_POINTERS info)
{
	BEE_LOG("<", errorLookup[info->ExceptionRecord->ExceptionCode & 0x3F], "> Exception code:",
		std::hex, info->ExceptionRecord->ExceptionCode, std::dec);
	bee::exceptionContext = info->ContextRecord;
	runtime.coredump();
	return EXCEPTION_CONTINUE_SEARCH;
}

#endif

Runtime::Runtime()
{
#	ifdef WIN32
	errorLookup[EXCEPTION_ACCESS_VIOLATION & 0x3F] = 
		"Access violation";
	errorLookup[EXCEPTION_ARRAY_BOUNDS_EXCEEDED & 0x3F] = 
		"Array bounds exceeded";
	errorLookup[EXCEPTION_BREAKPOINT & 0x3F] = 
		"Breakpoint";
	errorLookup[EXCEPTION_DATATYPE_MISALIGNMENT & 0x3F] = 
		"Datatype misalignment";
	errorLookup[EXCEPTION_FLT_DENORMAL_OPERAND & 0x3F] = 
		"Float denormal operand";
	errorLookup[EXCEPTION_FLT_DIVIDE_BY_ZERO & 0x3F] = 
		"Float divide by zero";
	errorLookup[EXCEPTION_FLT_INEXACT_RESULT & 0x3F] = 
		"Float inexact result";
	errorLookup[EXCEPTION_FLT_INVALID_OPERATION & 0x3F] = 
		"FLoat invalid operation";
	errorLookup[EXCEPTION_FLT_OVERFLOW & 0x3F] = 
		"Float overflow";
	errorLookup[EXCEPTION_FLT_STACK_CHECK & 0x3F] = 
		"Float stack check";
	errorLookup[EXCEPTION_FLT_UNDERFLOW & 0x3F] = 
		"Float underflow";
	errorLookup[EXCEPTION_ILLEGAL_INSTRUCTION & 0x3F] = 
		"Illegal instruction";
	errorLookup[EXCEPTION_IN_PAGE_ERROR & 0x3F] = 
		"In page error";
	errorLookup[EXCEPTION_INT_DIVIDE_BY_ZERO & 0x3F] = 
		"Integer divide by zero";
	errorLookup[EXCEPTION_INT_OVERFLOW & 0x3F] = 
		"Integer overflow";
	errorLookup[EXCEPTION_INVALID_DISPOSITION & 0x3F] = 
		"Invalid disposition";
	errorLookup[EXCEPTION_NONCONTINUABLE_EXCEPTION & 0x3F] = 
		"Noncontinuable exception";
	errorLookup[EXCEPTION_PRIV_INSTRUCTION & 0x3F] = 
		"Privilege instruction";
	errorLookup[EXCEPTION_SINGLE_STEP & 0x3F] = 
		"Single step";
	errorLookup[EXCEPTION_STACK_OVERFLOW & 0x3F] = 
		"Stack overflow";
	AddVectoredExceptionHandler(1, exception::veh);
	SetUnhandledExceptionFilter(handleException);
#	else
#		warn Coredump not fully supported on this platform.
#	endif
	if (haveInstance)
	{
		BEE_RAISE(Fatal, "Could not instantiate SINGLETON class 'bee::Runtime'.");
	}
	else
	{
		haveInstance = true;
	}
	initializeGraphics();
}

Runtime::~Runtime()
{
	finalizeGraphics();
}

void Runtime::createWindow(const std::string &title, bool fullscreen, unsigned width, unsigned height)
{
	BEE_EXCEPTION_BEGIN
		if (window)
		{
			BEE_RAISE(Fatal, "Could not instantiate SINGLETON object 'bee::runtime::window'.");
		}
		else
		{
			GLFWmonitor *pMonitor = nullptr;
			if (fullscreen)
			{
				pMonitor = glfwGetPrimaryMonitor();
				const GLFWvidmode *mode = glfwGetVideoMode(pMonitor);
				width = mode->width; height = mode->height;
			}
			window = glfwCreateWindow(width, height, title.c_str(), pMonitor, nullptr);
			util::width = width; util::height = height;
		}
	BEE_EXCEPTION_END
}

void Runtime::shell(const std::string &script)
{
	BEE_EXCEPTION_BEGIN
		using namespace np;
		using Ast = ast<int>;
		static reflected_lexer<Ast> lex(
			"+"_t, "-"_t, "*"_t, "/"_t, "~"_t, "%"_t, "<<"_t, ">>"_t, "&"_t, "^"_t, "|"_t, "("_t, ")"_t,
			"integer"_t = "(0[xX][0-9a-fA-F]+)|([1-9]\\d*)|(0[0-7]*)"_r
				>> lexer_reflect<Ast>([](const std::string& src){size_t idx; return stoll(src, &idx, 0);})
		);
		static parser<Ast> parser(lex,
			"start"_p = "expr"_p
			>> make_reflect<Ast>([](Ast& ast)->int{})
		);
	BEE_EXCEPTION_END
}

void Runtime::dump(std::string logFileName) noexcept
{
	auto writer(getDumpStream(logFileName));
	writer << dumpWriter.str() << std::endl;
}

void Runtime::coredump(std::string logFileName) noexcept
{
	auto writer(getDumpStream(logFileName));
	writer << "**bee** built " << __DATE__ << " " << __TIME__ << std::endl << std::endl;
	writer << dumpWriter.str() << std::endl;
	if (traceBack != "")
	{
		writer << traceBack;
	}
	else
	{
		stackTrace(writer, BEE_EXCEPTION_TRACE_DEPTH, 
#		ifdef WIN32
			exceptionContext
#		endif
		);
	}
	writer.close();
	std::terminate();
}

void Runtime::test()
{
	BEE_EXCEPTION_BEGIN
		while (!glfwWindowShouldClose(window)) {
			double currentFrame = glfwGetTime();
			// Utility::deltaTime = currentFrame - lastFrame;
			// lastFrame = currentFrame;
			// Input::processClick();

			// getScene()->update();

			// graphicsRuntime.update(getScene());
			BEE_LOG("this is a custom log");
			BEE_RAISE(ScriptError);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	BEE_EXCEPTION_END
}

// Private members

void Runtime::initializeGraphics() 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void Runtime::finalizeGraphics()
{
	glfwTerminate();
}

std::ofstream Runtime::getDumpStream(const std::string &logFileName)
{
#	ifdef WIN32
	if (access("log/", 02))
	{
		mkdir("log");
	}
#	else
#	endif
	if (logFileName == "")
	{
		return std::ofstream("log/" + genTimeStamp() + ".log");
	}
	return std::ofstream("log/" + logFileName);
}

}