#include "bee/runtime.h" 

int main()
{
	bee::runtime.exec([](){
		bee::runtime.createWindow("Header");
		// bee::runtime.shell("nothing");
		bee::runtime.test();
	});
	return 0;
}