#include "bee.h"

using namespace bee;
using namespace gl;
using namespace std;
using namespace glm;

int Main(int argc, char **argv)
{
	Window<3, 3> window;
	vector<ViewPort*> cameras;
	vector<Object*> objects;
	WaterSurface surface(2, 2);

	cameras.push_back(new FirstPersonCamera<>());
	objects.push_back(&surface);

	// surface.attachWave(pWave);
	// objects[0]->scale(0.02);
	cameras[0]->setPosition(0, -1, 0);
	cameras[0]->setTarget(0, 1, 0);
	
	ShaderControllers controllers;
	PointLight light(vec3(0, 2, 1));
	controllers.addController(light);
	UniformRef<float> gMatSpecularIntensity = Shader::uniform<float>("gMatSpecularIntensity");
	UniformRef<float> gSpecularPower = Shader::uniform<float>("gSpecularPower");

	window.dispatch<RenderEvent>(
		[&]() -> bool {
			for (auto & camera: cameras)
			{
				for (auto & object: objects)
				{
					object->render(*camera);
					controllers.invoke();
					gMatSpecularIntensity = 0.3;
					gSpecularPower = 32;
				}
			}
			return false;
		}
	);
	window.dispatch<KeyEvent>(
		[&](int key, int scancode, int action, int mods) -> bool {
			const auto step = .1f;
			switch (action) {
			case GLFW_PRESS: case GLFW_REPEAT:
				switch (key) {
				case GLFW_KEY_UP:
					cameras[0]->setPosition(cameras[0]->getPosition() + cameras[0]->getTarget() * step); break;
				case GLFW_KEY_DOWN:
					cameras[0]->setPosition(cameras[0]->getPosition() - cameras[0]->getTarget() * step); break;
				case GLFW_KEY_LEFT:

				case GLFW_KEY_RIGHT:;
				}
			}
			return false;
		}
	);
	window.dispatchMessages();
	return 0;
}