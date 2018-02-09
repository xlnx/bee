#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "runtime.h"
#include "util.h"

namespace bee
{

namespace exception
{

struct GLFatal: Fatal
{
	GLFatal(std::string info = "");
	GLFatal(GLenum err);
};

}

namespace gl
{

inline void checkError()
{
	if (auto err = glGetError())
	{
		BEE_RAISE(GLFatal, err);
	}
}

}

}
