#pragma once

#include "runtimeBase.h"
#include "util.h"

namespace bee
{

class Runtime final: public RuntimeBase
{
public:
	Runtime();
	~Runtime();

	void createWindow(const std::string &title = "bee~", 
			bool fullscreen = false, 
			unsigned width = 1024, 
			unsigned height = 768);
	void shell(const std::string &script);
	void test();
public:
	GLFWwindow *window = nullptr;
};

extern Runtime runtime;

}
