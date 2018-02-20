#include "bee.h"

using namespace std;
using namespace bee;
using namespace gl;
using namespace glm;

Window<3, 3> window;

int Main(int argc, char **argv)
{
	vector<ViewPort*> cameras;
	vector<Object*> objects;
	FirstPersonCamera<> camera;
	Flame flame;
	cameras.push_back(&camera);
	objects.push_back(&flame);
	window.dispatch<RenderEvent>(
		[&]() -> bool {
			for (auto &camera: cameras)
			{
				for (auto &object: objects)
				{
					object->render(*camera);
				}
			}
			return false;
		}
	);
	window.dispatchMessages();
	return 0;
}