#include "runtime.h"
#include "gl.h"
#include "window.h"
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
	// viewPorts[0].setPosition(-1, -1, 0);
	// viewPorts[0].setTarget(1, 1, 0);
	viewPorts[0].setPosition(0, -1, 0);
	viewPorts[0].setTarget(0, 1, 0);

	// viewPorts[1].setPosition(0, 0, -1);
	// viewPorts[1].setTarget(0, 0, 1);
	// viewPorts[1].setUp(0, 1, 0);
	model = Model("test.stl");
	object = Object(model, shader);
	object.scale(0.02, 0.02, 0.02);
	// object.translate(1, 0, 0);
	// window.dispatch<CursorPosEvent>(
	// [](double x, double y)->bool{
	// 	BEE_LOG(x, " ", y); return true;
	// });
	// window.dispatch<CursorPosEvent>(
	// [](double x, double y)->bool{
	// 	BEE_LOG("failed to stop"); return false;
	// }, -1);
	window.dispatch<KeyEvent>(
		[](int key, int scancode, int action, int mods) -> bool {
			const auto step = 0.02f;
			switch (action) {
			case GLFW_PRESS: case GLFW_REPEAT:
				switch (key) {
				case GLFW_KEY_UP:
					viewPorts[0].setPosition(viewPorts[0].getPosition() + viewPorts[0].getTarget() * step); break;
				case GLFW_KEY_DOWN:
					viewPorts[0].setPosition(viewPorts[0].getPosition() - viewPorts[0].getTarget() * step); break;
				case GLFW_KEY_LEFT:

				case GLFW_KEY_RIGHT:;
				}
			}
			return false;
		}
	);
	// viewPorts[0].setUp(viewPorts[0].getUp() + viewPorts[0].getTarget());
	window.dispatch<CursorPosEvent>(
		[](double x, double y) -> bool {
			const auto step = 0.002f;
			const auto detX = x - window.getCursorX();
			const auto detY = y - window.getCursorY();

			static auto angleH = 0.f;
			static auto angleV = 0.f;
			angleH = detX * step;
			angleV = detY * step;

			auto sinx = sinf(angleH / 2);
			auto cosx = cosf(angleH / 2);
			auto up = viewPorts[0].getUp();
			viewPorts[0].setTarget(::glm::rotate(
				::glm::quat(cosx, up.x * sinx, up.y * sinx, up.z * sinx), viewPorts[0].getTarget()));
			return false;
		}
	);

	while (!window.closed())
	{
		render();
		checkError();
		
		window.swapBuffers();
		window.pollEvents();
	}

	return 0;
}