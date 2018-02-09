#include "runtime.h"
#include "gl.h"

using namespace bee;
using namespace bee::gl;

Window window("testMesh", false, 512, 512);

Model model;

UniformRef<::glm::mat4> world;

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;
	Scale += 0.001f;
	::glm::mat4 rotatey = {
		cosf(Scale), 0.f, -sinf(Scale), 0.f,
		0.f, 1.f, 0.f, 0.f,
		sinf(Scale), 0.f, cosf(Scale), 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	world = rotatey; // * rotatex; // * scale;

	model.render();
}

int Main(int argc, char **argv)
{
	::bee::gl::Shader shader{
		::bee::gl::VertexShader("testModel.vert"),
		::bee::gl::FragmentShader("testModel.frag")
	};
	shader.use();
	world.bind(shader.uniform<::glm::mat4>("gWorld"));
	model = Model("C:/Users/QwQ/Desktop/bee/build/tests/test.stl");
	while (!window.closed())
	{
		render();
		checkError();
		
		window.swapBuffers();
		window.pollEvents();
	}
	return 0;
}