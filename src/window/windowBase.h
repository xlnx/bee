#pragma once

#include "common.h"
#include <GLFW/glfw3.h>

namespace bee
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
	static unsigned getWidth()
	{
		return width;
	}
	static unsigned getHeight()
	{
		return height;
	}
protected:
	GLFWwindow *window;
	static unsigned width, height;
	static WindowBase *instance;
};

}