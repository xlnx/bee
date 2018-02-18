#include "bee.h"

using namespace bee;
using namespace gl;
using namespace glm;
using namespace std;

int Main(int argc, char **argv)
{
	Configure c("config.json");
	c["main"] = Json::arrayValue;
	c["array[5]"] = 2;
}