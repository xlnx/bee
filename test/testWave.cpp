#include "bee.h"

using namespace std;
using namespace bee;
using namespace gl;
using namespace glm;

Window<3, 3> window;
vector<ViewPort*> cameras;
Scene scene;
UniformRef<float> gMatSpecularIntensity = Shader::uniform<float>("gMatSpecularIntensity");
UniformRef<float> gSpecularPower = Shader::uniform<float>("gSpecularPower");

bool render()
{
	for (auto &camera: cameras)
	{
		scene.render(*camera);
		// gMatSpecularIntensity = 0.3;
		// gSpecularPower = 32;
	}
	return false;
}

int Main(int argc, char **argv)
{
	GerstnerWave hWave, vWave, sWave, pWave; 
	// vector<Object*> objects;
	auto &surface = scene.createObject<WaterSurface>(2, 2);

	FirstPersonCamera<> camera;
	cameras.push_back(&camera);

	surface.attachWave(hWave);
	surface.attachWave(vWave);
	surface.attachWave(sWave);
	// surface.attachWave(pWave);
	
	hWave.setDirection(1, 0);
	vWave.setDirection(0, 1);
	sWave.setDirection(1, 1.5);
	pWave.setDirection(-1, 1.2);
	pWave.setFrequency(2.f);
	pWave.setSteepness(1.f);
	vWave.setAmplitude(vWave.getAmplitude() / 2);
	// objects[0]->scale(0.02);
	camera.setPosition(0, -1, 0);
	camera.setTarget(0, 1, 0);
	// surface.setPosition(-1, 0, 0);
	
	scene.createController<DirectionalLight>(vec3(0, -1, -1));
	CameraCarrier cc(camera);
	window.dispatch<RenderEvent>(render);
	window.dispatchMessages();
	return 0;
}