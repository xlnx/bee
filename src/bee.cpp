#include "runtime.h"
#include "gl.h"

int main()
{
	::bee::Runtime::exec([](){
		::bee::gl::Window window("Header");

		
		// bee::Runtime::shell("nothing");
		// std::cout << 1 / 0 << std::endl;
	});
	return 0;
}