#include "gl.h"
#include "runtime.h"
#include "pipeline.h"
#include "shading.h"
#include "window.h"
#include "waterSurface.h"

using namespace std;
using namespace bee;
using namespace gl;
using namespace glm;

int Main(int argc, char **argv)
{
	Window<3, 3> window;
	vector<ViewPort*> cameras;
	vector<Object*> objects;
	auto ptr = new WaterSurface();
	cameras.push_back(new FirstPersonCamera<>());
	objects.push_back(ptr);
	ptr->setShader(new Shader(
		VertexShader("testWave.vert"),
		FragmentShader("testWave.frag")
	));
	ptr->attachWave(* new GerstnerWave());
	ptr->resize(2, 2);
	// objects[0]->scale(0.02);
	cameras[0]->setPosition(0, -1, 0);
	cameras[0]->setTarget(0, 1, 0);
	ShaderControllers<> controllers;
	controllers.addController(* new DirectionalLight(vec3(1, -1, -1)));
	window.dispatch<RenderEvent>(
		[&]() -> bool {
			for (auto & camera: cameras)
			{
				for (auto & object: objects)
				{
					object->render(*camera);
					controllers.invoke();
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