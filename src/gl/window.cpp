#include "window.h"

namespace bee
{

namespace gl
{

static const char *errorMessage[] = {
	"",
	"GL_INVALID_ENUM",
	"GL_INVALID_VALUE",
	"GL_INVALID_OPERATION"
	"GL_STACK_OVERFLOW",
	"GL_STACK_UNDERFLOW",
	"GL_OUT_OF_MEMORY",
	"GL_INVALID_FRAMEBUFFER_OPERATION",
	"GL_CONTEXT_LOST"
};

}

namespace exception
{

GLFatal::GLFatal(std::string info):
	Fatal(info)
{
}

GLFatal::GLFatal(GLenum err):
	Fatal(static_cast<std::ostringstream&>(std::ostringstream() << 
		"glGetError() = 0x" << std::hex << err << std::dec << ": " << gl::errorMessage[err & 0xff]).str())
{
}

}

namespace gl
{

GLFWwindow *Window::window;

Window::Window(const std::string &title, bool fullscreen, unsigned width, unsigned height)
{
	if (window)
	{
		BEE_RAISE(Fatal, "Could not instantiate SINGLETON object 'bee::runtime::window'.");
	}

	Runtime::onCoredump([]() noexcept
	{
		if (auto err = glGetError())
			BEE_LOG("glGetError() = 0x", std::hex, err, std::dec, ": ", errorMessage[err & 0xff]);
		glfwTerminate();
	});
	if (!glfwInit())
	{
		BEE_RAISE(GLFatal, "GLFW failed to init!");
	}
	glfwSetErrorCallback([](int errCode, const char *log)
	{
		std::ostringstream os;
		os << "GLError 0x" << std::hex << errCode << ": " << log;
		BEE_RAISE(GLFatal, os.str());
	});
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWmonitor *pMonitor = nullptr;
	if (fullscreen)
	{
		pMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(pMonitor);
		width = mode->width; height = mode->height;
	}
	window = glfwCreateWindow(width, height, title.c_str(), pMonitor, nullptr);
	util::width = width; util::height = height;
	glfwMakeContextCurrent(window);
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::ostringstream os;
		os << "glewInit failed: " << glewGetErrorString(err);
		BEE_RAISE(GLFatal, os.str());
	}
}

Window::~Window()
{
	glfwTerminate();
}

void exec(std::function<void()> fn)
{
	fn();
	if (auto err = glGetError())
	{
		BEE_RAISE(GLFatal, err);
	}
}

}

}
