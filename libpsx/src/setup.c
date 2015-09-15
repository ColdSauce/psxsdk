#include <psx.h>
#include <stdio.h>
#include "memory.h"

extern int __bss_start[];
extern int __bss_end[];
extern void *__ctor_list;
extern void *__ctor_end;

// Function to call static constructors (for C++, etc.)
static void call_ctors(void)
{
	dprintf("Calling static constructors..\n");
	
	void **p = &__ctor_list;
	
        for (++p; *p != NULL && p < &__ctor_end; p++)
                (*(void (**)())p)();
	
	dprintf("Finished calling static constructors\n");
 }

void psxsdk_setup()
{
	unsigned int x;

	printf("Initializing PSXSDK... \n");

// Clear BSS space	
	for(x = (unsigned int)__bss_start; x < (unsigned int)__bss_end; x++)
		*((unsigned char*)x) = 0;

// Setup memory allocation functions
	malloc_setup();

// Call static constructors
	call_ctors();
}
