#pragma once

#ifdef BEE_OPENGL_LOADER_GLAD
#include <glad/glad.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "runtime.h"

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
