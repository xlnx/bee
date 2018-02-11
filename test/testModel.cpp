#include "runtime.h"
#include "gl.h"
#include "object.h"
#include <vector>

using namespace bee;
using namespace bee::gl;

Window<3, 3> window("testModel", false, 512, 512);

Model model;

Object object;

::bee::gl::Shader shader{
	::bee::gl::VertexShader("testModel.vert"),
	::bee::gl::FragmentShader("testModel.frag")
};

::std::vector<ViewPort> viewPorts;

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static auto angle = 0.002f;
	static auto s = 0.f;
	s += 0.002f;
	// object.rotate(angle, angle * 2, angle * 3);
	// object.translate(0, 0, angle);
	for (auto &viewPort: viewPorts)
	{
		object.render(viewPort);
	}
}

int Main(int argc, char **argv)
{
	viewPorts.emplace_back();
	// viewPorts.emplace_back(0, 0, 200, 200);
	viewPorts[0].setPosition(-1, -1, 0);
	viewPorts[0].setTarget(1, 1, 0);
	// viewPorts[1].setTarget(0, 0, -1);
	// viewPorts[1].setUp(0, 1, 0);
	model = Model("test.stl");
	object = Object(model, shader);
	object.scale(0.02, 0.02, 0.02);
	// object.translate(1, 0, 0);

	while (!window.closed())
	{
		render();
		checkError();
		
		window.swapBuffers();
		window.pollEvents();
	}
	return 0;
}