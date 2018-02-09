#pragma once

#include "common.h"

namespace bee
{

namespace gl
{

class Window 
{
public:
	Window(const std::string &title = "bee~", 
			bool fullscreen = false, 
			unsigned width = 1024, 
			unsigned height = 768);
	Window(const Window &other) = delete;
	virtual ~Window();

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
private:
	static GLFWwindow *window;
};

}

}
