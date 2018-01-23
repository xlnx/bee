#include "runtime.h"
#include "../../dependency/new_parser/parser.h"

namespace bee
{

Runtime::Runtime()
{
	if (haveInstance)
	{
		throw Fatal("Could not instantiate SINGLETON class 'bee::Runtime'.");
	}
	else
	{
		haveInstance = true;
	}
	initializeGraphics();
}

~Runtime()
{
	finalizeGraphics();
}

void Runtime::createWindow(const std::string &title, bool fullscreen, unsigned width, unsigned height)
{
	if (window)
	{
		throw Fatal("Could not instantiate SINGLETON object 'bee::runtime::window'.");
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

void Runtime::log()
{
	
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

}