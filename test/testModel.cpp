#include "runtime.h"
#include "gl.h"
#include "object.h"
#include <vector>

using namespace bee;
using namespace bee::gl;

Window window("testModel", false, 512, 512);

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

void printmat(const ::glm::mat4 &mat)
{
	::std::cout << "{" << ::std::endl;
	for (int i = 0; i != 4; ++i)
	{
		for (int j = 0; j != 4; ++j)
		{
			::std::cout << mat[i][j] << " ";
		}
		::std::cout << ::std::endl;
	}
	::std::cout << "}" << ::std::endl;
}

void printvec(const ::glm::vec4 &vec)
{
	::std::cout << "{";
	// for (int i = 0; i != 4; ++i)
	// {
		for (int j = 0; j != 4; ++j)
		{
			::std::cout << vec[j] << " ";
		}
	// }
	::std::cout << "}" << ::std::endl;
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

	::glm::mat4 a = {
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		1.f, 1.f, 1.f, 1.f
	}, b = {
		1.f, 0.f, 0.f, 0.f,
		1.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f	
	};
	::glm::vec4 v = {
		0, 0, 0, 1
	};

	printmat(::glm::transpose(a) * ::glm::transpose(b));
	printvec(a * v);

	while (!window.closed())
	{
		render();
		checkError();
		
		window.swapBuffers();
		window.pollEvents();
	}
	return 0;
}