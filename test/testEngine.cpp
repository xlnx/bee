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

	auto &surface = scene.createObject<WaterSurface>(60, 60);
	GerstnerWave wave[5];
	surface.attachWave(wave[0]);
	surface.attachWave(wave[1]);
	surface.attachWave(wave[2]);
	surface.attachWave(wave[3]);
	surface.attachWave(wave[4]);
	// surface.attachWave(pWave);
	
	wave[0].setDirection(1, 0);
	wave[1].setDirection(0, 1);
	wave[2].setDirection(1, 1.5);
	wave[3].setDirection(-1, 1);
	wave[4].setDirection(1, -.8);
	
	wave[0].setFrequency(2.5f);
	wave[1].setFrequency(2.f);
	wave[2].setFrequency(.5f);
	wave[3].setFrequency(1.f);
	wave[4].setFrequency(.1f);
	
	wave[0].setAmplitude(.1f);
	wave[1].setAmplitude(.09f);
	wave[2].setAmplitude(.12f);
	wave[3].setAmplitude(.08f);
	wave[4].setAmplitude(.5f);

	wave[0].setSteepness(.7f);
	wave[1].setSteepness(.5f);
	wave[2].setSteepness(.3f);
	wave[3].setSteepness(.1f);
	wave[4].setSteepness(.0f);

	scene.createObject<ModelObject>(
		ModelManager::loadModel("NBB_Bismark/NBB_Bismark.obj")
	);
	scene.createObject<Skybox>();
	auto &light = scene.createController<DirectionalLight>(
		vec3{0, -.1, -1}
	);
	// light.setDiffuseIntensity(.3f);
	CameraCarrier cc(camera);
	window.dispatch<RenderEvent>(render);
	window.dispatchMessages();
	return 0;
}