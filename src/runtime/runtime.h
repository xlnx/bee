#pragma once

#include "exception.h"
#include "../util.h"

namespace bee
{

class Runtime final
{
public:
	Runtime();
	~Runtime();

	void createWindow(const std::string &title = "bee~", 
			bool fullscreen = false, 
			unsigned width = 1024, 
			unsigned height = 768);
	void shell(const std::string &script);
	void log();
private:
	void initializeGraphics();
	void finalizeGraphics();

public:
	GLFWwindow *window = nullptr;
private:
	static bool haveInstance;
} runtime;

bool Runtime::haveInstance = false;

}