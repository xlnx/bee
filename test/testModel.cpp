#include "runtime.h"
#include "gl.h"
#include "pipeline.h"
#include "shading.h"
#include "window.h"

using namespace bee;
using namespace bee::gl;
using namespace std;
using namespace glm;

Window<3, 3> window("testModel", false, 512, 512);
Model model;
Object object;
vector<ViewPort*> viewPorts;

int Main(int argc, char **argv)
{
	Shader shader{
		VertexShader("testModel.vert"),
		FragmentShader("testModel.frag")
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
	::std::string line;
	::std::cin >> line;
	model = Model(line);
	object = Object(model, shader);
	object.scale(0.02, 0.02, 0.02);

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
	vector<ShaderController *> controllers;
	controllers.emplace_back(new DirectionalLight(vec3(1, 1, 1)));
	// controllers.emplace_back(new PointLight(vec3(0, 0, 1)));
	window.dispatch<RenderEvent>(
		[&]() -> bool {
			static auto angle = 0.002f;
			static auto s = 0.f;
			s += 0.002f;
			// object.rotate(angle, angle * 2, angle * 3);
			// object.translate(0, 0, angle);
			for (auto &viewPort: viewPorts)
			{
				object.render(*viewPort);
				for (auto &controller: controllers) 
				{
					controller->invoke();
				}
			}
			return false;
		}
	);
	window.dispatchMessages();
	return 0;
}