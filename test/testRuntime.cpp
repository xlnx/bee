#include "bee/runtime.h" 

void badboy()
{
	((int*)258)[23] = 124;
}

void badfather()
{
	badboy();
}

int main()
{
	bee::runtime.createWindow("Header");
	// bee::runtime.shell("nothing");
	// std::cout << 1 / 0 << std::endl;
	badfather();
	bee::runtime.test();
	return 0;
}