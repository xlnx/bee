#include "bee.h"

using namespace bee;
using namespace gl;
using namespace std;
using namespace glm;

Window<3, 3> window("testSkeletalAnimation", false, 1024, 1024);

int Main(int argc, char *argv[])
{
	Scene scene;
	auto camera = scene.create<FirstPersonCamera<>>();
	camera->setPosition(2, 2, 3);
	camera->setTarget(-0.5, -0.5, -0.8);
	CameraCarrier cc(*camera);

	auto light = scene.create<PointLight>();
	light->setDiffuseIntensity(2.8f);
	light->setPosition(0, 0, 2);
	scene.setMajorLight(*light);
	auto object = scene.create<ModelObject>("chess/testSkeletalAnimation.obj");
	object->scale(0.025);
	object->translate(0, 0, -0.4);

	window.dispatch<RenderEvent>([&]() -> bool{
		scene.shadowPass();
		scene.renderPass();
		return false;
	});
	window.dispatchMessages();
	return 0;
}
