#include "bee.h"

using namespace std;
using namespace bee;
using namespace gl;
using namespace glm;

Window<3, 3> window;
vector<ViewPort*> cameras;
Scene scene;

bool render()
{
	for (auto &camera: cameras)
	{
		scene.render(*camera);
	}
	return false;
}

int Main(int argc, char **argv)
{
	FirstPersonCamera<> camera;
	cameras.push_back(&camera);

	scene.createObject<ModelObject>(
		ModelManager::loadModel("NBB_Bismark/NBB_Bismark.obj")
	);
	scene.createController<DirectionalLight>(
		vec3{1.0, 2.0, 3.0}
	);
	window.dispatch<RenderEvent>(render);
	window.dispatchMessages();
	return 0;
}