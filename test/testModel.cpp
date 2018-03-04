#include "bee.h"

using namespace bee;
using namespace bee::gl;
using namespace std;
using namespace glm;

Window<3, 3> window("testModel", false, 512, 512);
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
	camera.setPosition(0, -1, 0);
	camera.setTarget(0, 1, 0);
	CameraCarrier cc(camera);

	scene.createController<PointLight>(vec3(0, 0, 2));
	scene.createObject<ModelObject>("chess/chess.obj")//"NBB_Bismark/NBB_Bismark.obj")
		.scale(0.02, 0.02, 0.02);
	scene.createObject<OceanMesh>();
	window.dispatch<RenderEvent>(render);
	window.dispatchMessages();
	return 0;
}