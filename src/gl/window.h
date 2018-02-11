#pragma once

#include "common.h"

namespace bee
{

namespace gl
{

class WindowBase 
{
public:
	WindowBase(const std::string &title = "bee~", 
			bool fullscreen = false, 
			unsigned width = 1024, 
			unsigned height = 768);
	WindowBase(const WindowBase &other) = delete;
	virtual ~WindowBase();

	bool closed() const
	{
		return glfwWindowShouldClose(window);
	}
	void pollEvents() const
	{
		glfwPollEvents();
	}
	void swapBuffers() const
	{
		glfwSwapBuffers(window);
	}
	operator GLFWwindow *() const
	{
		return window;
	}
protected:
	static GLFWwindow *window;
};

template <int Major, int Minor>
class Window: public WindowBase
{
public:
	template <typename ...Types, typename = typename
		::std::enable_if<::std::is_constructible<WindowBase, Types...>::value>::type >
	Window(Types &&...args): 
		WindowBase(::std::forward<Types>(args)...)
	{
		// Specify OpenGL Version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Minor);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glClearColor(0.f, 0.f, 0.f, 0.f);
	}

	// void processMouseEvent() const
	// {
	// 	glfwGetMouseButton(Utility::window, GLFW_MOUSE_BUTTON_1)
	// }
};

}

}
