#include "runtime.h" 

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
	badfather();
	return 0;
}