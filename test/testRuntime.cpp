#include "runtime.h" 
#include <cassert>

void badboy()
{
	((int*)258)[23] = 124;
}

void badfather()
{
	badboy();
}

int Main(int argc, char **argv)
{
	// std::cout << 1 / 0 << std::endl;
	// assert(false);
	badfather();
	return 0;
}