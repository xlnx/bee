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

MyWindow window("testGL", false, 512, 256);

GLuint VBO;

::bee::gl::Ref<::glm::mat4> world;

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;
	Scale += 0.001f;
	// glUniform1f(gScaleLocation, sinf(Scale));
	world = {
		1.f, 0.f, 0.f, sinf(Scale),
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void init()
{
	::glm::vec3 vertices[] = {
		{-1.f, -1.f, 0.f},
		{1.f, -1.f, 0.f},
		{0.f, 1.f, 0.f}
	};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// TODO: Perform more shader Usage
	::bee::gl::Shader shader{
		::bee::gl::VertexShader("test.vert"),
		::bee::gl::FragmentShader("test.frag")
	};
	shader.use();
	// glUseProgram(shader);
	world.bind(shader.var<::glm::mat4>("gWorld"));
}

int Main(int argc, char **argv)
{
	// -DBEE_RUNTIME_INTRUSIVE
	// ::bee::Runtime::exec([]()
	// {
	init();
	while (!glfwWindowShouldClose(window))
	{
		bee::gl::exec(render);
	}
	// });
	return 0;
}