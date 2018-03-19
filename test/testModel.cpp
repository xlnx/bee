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
	// camera.setPosition(0, 0, 2);
	camera.setTarget(-1, 0, -0.5);
	// camera.setTarget(-0.01, 0, -1);
	CameraCarrier cc(camera);

	if (argc < 2) return 1;
	scene.setMajorLight(scene.createController<PointLight>(vec3(0, 0, 2)));
	auto &object = scene.createObject<ModelObject>(argv[1]);
		// ("chess/chess.obj");
	if (argc >= 3)
	{
		float scale = stof(string(argv[2]));
		object.scale(scale, scale, scale);
	}
	// auto &board = scene.createObject<ModelObject>("board/board.obj");
	// board.scale(0.02, 0.02, 0.02);
	// object.rotate(::glm::radians(180.f), 0, 0);
	// object.translate(0, 0, 2);
	// scene.createObject<OceanMesh>();
	window.dispatch<RenderEvent>([&]() -> bool{
		scene.shadowPass();
		scene.renderPass();
		return false;
	});
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	window.dispatchMessages();
	return 0;
}
