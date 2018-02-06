#include "runtime.h"
#include "gl.h"

struct MyWindow: ::bee::gl::Window
{
public:
	template <typename ...Types>
		MyWindow(Types &&...args):
			::bee::gl::Window(std::forward<Types>(args)...)
	{
		// Specify OpenGL Version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glClearColor(0.f, 0.f, 0.f, 0.f);
	}
};

const ::bee::gl::VertexAttrs<::bee::gl::color3, ::bee::gl::pos3> vertices = {
	{ {1, 2, 3}, {-1, -1, 0} },
	{ {1, 2, 3}, {0, -1, 1} },
	{ {1, 2, 3}, {1, -1, 0} },
	{ {1, 2, 3}, {0, 1, 0} },
};
const ::bee::gl::Indices indices = {
	{ 0, 3, 1 },
	{ 1, 3, 2 },
	{ 0, 3, 2 },
	{ 0, 1, 2 }
};

MyWindow window("testGL", false, 512, 256);

::bee::gl::UniformRef<::glm::mat4> world;

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;
	Scale += 0.001f;
	// move
	::glm::mat4 move = {
		1.f, 0.f, 0.f, sinf(Scale),
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	// rotate z
	::glm::mat4 rotatez = {
		cosf(Scale), -sinf(Scale), 0.f, 0.f,
		sinf(Scale), cosf(Scale), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	// rotate y
	::glm::mat4 rotatey = {
		cosf(Scale), 0.f, -sinf(Scale), 0.f,
		0.f, 1.f, 0.f, 0.f,
		sinf(Scale), 0.f, cosf(Scale), 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	// rotate x
	::glm::mat4 rotatex = {
		1.f, 0.f, 0.f, 0.f,
		0.f, cosf(Scale), -sinf(Scale), 0.f,
		0.f, sinf(Scale), cosf(Scale), 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	::glm::mat4 scale = {
		cosf(Scale), 0.f, 0.f, 0.f,
		0.f, cosf(Scale), 0.f, 0.f,
		0.f, 0.f, cosf(Scale), 0.f, 
		0.f, 0.f, 0.f, 1.f
	};
	world = rotatey; // * rotatex; // * scale;

	static ::bee::gl::Mesh mesh(vertices, indices);

	mesh.render();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void init()
{
	// TODO: Perform more shader Usage
	::bee::gl::Shader shader{
		::bee::gl::VertexShader("test.vert"),
		::bee::gl::FragmentShader("test.frag")
	};
	shader.use();
	// glUseProgram(shader);
	world.bind(shader.uniform<::glm::mat4>("gWorld"));
}

int Main(int argc, char **argv)
{
	init();
	while (!glfwWindowShouldClose(window))
	{
		bee::gl::exec(render);
	}
	return 0;
}