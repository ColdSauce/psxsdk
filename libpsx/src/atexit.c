#include <stdlib.h>

static void *f_ptr[32];
static int f_ptr_pos = 0;

int atexit(void (*function)(void))
{
	if(f_ptr_pos >= 32)
		return -1;
	
	f_ptr[f_ptr_pos++] = function;

	return 0;
}

void call_atexit_callbacks(void)
{
	int i;
	void (*f)(void);
	
	for(i = (f_ptr_pos - 1); i >= 0; i--)
		(f = f_ptr[i])();
}
