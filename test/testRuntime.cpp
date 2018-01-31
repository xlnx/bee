#include "runtime.h" 

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
	// std::cout << 1 / 0 << std::endl;
	badfather();
	return 0;
}