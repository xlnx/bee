#include "runtime.h"
#include "gl.h"
#include "window.h"
#include "object.h"
#include "firstPersonCamera.h"
#include <vector>

using namespace bee;
using namespace bee::gl;

Window<3, 3> window("testModel", false, 512, 512);
Model model;
Object object;
::std::vector<ViewPort*> viewPorts;

int Main(int argc, char **argv)
{
	::bee::gl::Shader shader{
		::bee::gl::VertexShader("testModel.vert"),
		::bee::gl::FragmentShader("testModel.frag")
	};
	viewPorts.emplace_back(new FirstPersonCamera<>());
	// viewPorts.emplace_back(0, 0, 200, 200);
	// viewPorts[0].setPosition(-1, -1, 0);
	// viewPorts[0].setTarget(1, 1, 0);
	viewPorts[0]->setPosition(0, -1, 0);
	viewPorts[0]->setTarget(0, 1, 0);

	// viewPorts[1].setPosition(0, 0, -1);
	// viewPorts[1].setTarget(0, 0, 1);
	// viewPorts[1].setUp(0, 1, 0);
	model = Model("test.obj");
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
			const auto step = .1f;
			switch (action) {
			case GLFW_PRESS: case GLFW_REPEAT:
				switch (key) {
				case GLFW_KEY_UP:
					viewPorts[0]->setPosition(viewPorts[0]->getPosition() + viewPorts[0]->getTarget() * step); break;
				case GLFW_KEY_DOWN:
					viewPorts[0]->setPosition(viewPorts[0]->getPosition() - viewPorts[0]->getTarget() * step); break;
				case GLFW_KEY_LEFT:

				case GLFW_KEY_RIGHT:;
				}
			}
			return false;
		}
	);
	window.dispatch<RenderEvent>(
		[]() -> bool {
			glClear(GL_COLOR_BUFFER_BIT);

			static auto angle = 0.002f;
			static auto s = 0.f;
			s += 0.002f;
			// object.rotate(angle, angle * 2, angle * 3);
			// object.translate(0, 0, angle);
			for (auto &viewPort: viewPorts)
			{
				object.render(*viewPort);
			}
			return false;
		}
	);
	window.dispatchMessages();
	return 0;
}