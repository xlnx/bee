#include "windowBase.h"

namespace bee
{

static const char *errorMessage[] = {
	"GL_INVALID_ENUM",
	"GL_INVALID_VALUE",
	"GL_INVALID_OPERATION",
	"GL_STACK_OVERFLOW",
	"GL_STACK_UNDERFLOW",
	"GL_OUT_OF_MEMORY",
	"GL_INVALID_FRAMEBUFFER_OPERATION",
	"GL_CONTEXT_LOST",
};

namespace exception
{

GLFatal::GLFatal(std::string info):
	Fatal(info)
{
}

static ::std::string getGLErrorMessage(GLenum err)
{
	::std::ostringstream os;
	os << "glGetError() = 0x" << std::hex << err << std::dec << ": " << errorMessage[err & 0xff];
	return os.str();
}

GLFatal::GLFatal(GLenum err):
	Fatal(getGLErrorMessage(err))
{
}

}

WindowBase *WindowBase::instance = nullptr;

unsigned WindowBase::width = 0;

unsigned WindowBase::height = 0;

WindowBase::WindowBase(int major, int minor, const std::string &title, bool fullscreen, unsigned width, unsigned height)
{
	if (instance)
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_SAMPLES, 4);
#	ifdef BEE_OPENGL_CORE_PROFILE
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#	endif
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWmonitor *pMonitor = nullptr;
	if (fullscreen)
	{
		pMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(pMonitor);
		width = mode->width; height = mode->height;
	}
	window = glfwCreateWindow(width, height, title.c_str(), pMonitor, nullptr);
	this->width = width; this->height = height;
	glfwMakeContextCurrent(window);
#	ifdef BEE_OPENGL_LOADER_GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		BEE_RAISE(GLFatal, "Failed to initialize GLAD");
	}
#	else
	if (glewInit())
	{
		BEE_RAISE(GLFatal, "Failed to initialize GLEW");
	}
#	endif
	instance = this;
}

WindowBase::~WindowBase()
{
	glfwTerminate();
}

}
