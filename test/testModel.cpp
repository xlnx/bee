#include "runtime.h"
#include "gl.h"
#include "object.h"

using namespace bee;
using namespace bee::gl;

Window window("testMesh", false, 512, 512);

Model model;

Object object;

::bee::gl::Shader shader{
	::bee::gl::VertexShader("testModel.vert"),
	::bee::gl::FragmentShader("testModel.frag")
};

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static auto angle = 0.002f;
	object.rotate(angle, angle * 2, angle * 3);
	object.render();
}

int Main(int argc, char **argv)
{
	model = Model("C:/Users/QwQ/Desktop/bee/build/tests/test.stl");
	object = Object(model, shader);
	object.scale(0.02, 0.02, 0.02);
	// for (int i = 0; i != 4; ++i)
	// 	for (int j = 0; j != 4; ++j)
	// 		BEE_LOG(camera.getTrans()[i][j]);
	// 	BEE_LOG();
	// BEE_LOG();
	// for (int i = 0; i != 4; ++i)
	// 	for (int j = 0; j != 4; ++j)
	// 		BEE_LOG(object.getTrans()[i][j]);
	// 	BEE_LOG();
	while (!window.closed())
	{
		render();
		checkError();
		
		window.swapBuffers();
		window.pollEvents();
	}
	return 0;
}