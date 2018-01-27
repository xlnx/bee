#include "bee/runtime.h"
#include "new_parser/parser.h"

namespace bee
{

Runtime runtime;

Runtime::Runtime()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

Runtime::~Runtime()
{
	glfwTerminate();
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
		BEE_RAISE(Fatal);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
BEE_EXCEPTION_END
}

}
