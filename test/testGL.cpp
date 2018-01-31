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
		// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glClearColor(0.f, 0.f, 0.f, 0.f);
	}
};

int main()
{
	::bee::Runtime::exec([]()
	{
		MyWindow window("testGL");
		GLuint VBO;
		::glm::vec3 vertices[] = {
			{-1.f, -1.f, 0.f},
			{1.f, -1.f, 0.f},
			{0.f, 1.f, 0.f}
		};
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		while (!glfwWindowShouldClose(window))
		{
			bee::gl::exec([&]()
			{
				glClear(GL_COLOR_BUFFER_BIT);
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
					glDrawArrays(GL_TRIANGLES, 0, 3);
				glDisableVertexAttribArray(0);

				glfwSwapBuffers(window);
				glfwPollEvents();
			});
		}
	});
	return 0;
}