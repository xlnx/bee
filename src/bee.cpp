#include "runtime.h"
#include "gl.h"
#include "window.h"

using namespace ::bee;
using namespace ::bee::gl;

int Main(int argc, char **argv)
{
	// -DBEE_RUNTIME_INTRUSIVE
	// ::bee::Runtime::exec([](){
	Window<3, 3> window("Header");

	
	// bee::Runtime::shell("nothing");
	// std::cout << 1 / 0 << std::endl;
	// });
	return 0;
}