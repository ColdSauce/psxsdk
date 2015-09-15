extern "C"
{
	#include <stdio.h>
	#include <stdlib.h>
}

extern "C" void __cxa_pure_virtual()
{
	printf("\nError: Called pure virtual C++ function!\n");
	exit(-1);
}
