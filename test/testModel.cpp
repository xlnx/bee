#include "bee.h"

using namespace bee;
using namespace bee::gl;
using namespace std;
using namespace glm;

Window<3, 3> window("testModel", false, 512, 512);

int Main(int argc, char **argv)
{
	Scene scene;
	auto &camera = scene.createCamera<FirstPersonCamera<>>();
	camera.setPosition(4, 0, 2);
	camera.setTarget(-1, 0, -0.5);
	CameraCarrier cc(camera);

	scene.setMajorLight(scene.createController<PointLight>(vec3(0, 0, 2)));
	scene.createObject<ModelObject>("chess/chess.obj")
		.scale(0.02, 0.02, 0.02);
	// scene.createObject<OceanMesh>();
	window.dispatch<RenderEvent>([&]() -> bool{
		scene.renderDepth();
		scene.render();
		return false;
	});
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	window.dispatchMessages();
	return 0;
}