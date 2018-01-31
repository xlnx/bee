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
	operator GLFWwindow *() const
	{
		return window;
	}
private:
	static GLFWwindow *window;
};

void exec(std::function<void()> fn);

}

}
