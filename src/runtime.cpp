#include <ctime>
#include "bee/runtime.h"
#include "new_parser/parser.h"

namespace bee
{

bool Runtime::haveInstance = false;

Runtime runtime;

Runtime::Runtime()
{
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
}

void Runtime::shell(const std::string &script)
{
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

}

std::string Runtime::genTimeStamp()
{
	time_t timep;
	char tmp[128];
	time(&timep);
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H-%M-%S", localtime(&timep));
	return tmp;
}

// void Runtime::log(const std::string &message)
// {
// 	auto s = genTimeStamp();
// 	std::cerr << "[" << s << "] " << message << std::endl;
// 	dumpWriter << "[" << s << "] " << message << std::endl;
// }

void Runtime::dump(std::string logFileName)
{
	if (logFileName == "")
	{
		logFileName = genTimeStamp() + ".log";
	}
	std::ofstream fout(logFileName);
	dumpUtil(fout);
}

void Runtime::coreDump(std::string logFileName)
{
	if (logFileName == "")
	{
		logFileName = genTimeStamp() + ".log";
	}
	std::ofstream fout(logFileName);
	fout << "**bee** built " << __DATE__ << " " << __TIME__ << std::endl;
	dumpUtil(fout);
	fout.close();
	std::terminate();
}

void Runtime::test()
{
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
}

void Runtime::exec(const std::function<void()> &fn)
{
	try
	{
		fn();
	}
	catch (exception::Fatal &e)
	{
		log(e.what());
		coreDump();
	}
	catch (std::exception &e)
	{
		log("This application has been trapped in an unexpected condition.");
		coreDump();
	}
}

// private members

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

void Runtime::dumpUtil(std::ofstream &of)
{
	of << dumpWriter.str();
}

}