#include <stdlib.h>
#include <unistd.h>

void swab(const void *src, void *dst, ssize_t len)
{
	ssize_t x;
	const unsigned char *srcp = src;
	unsigned char *dstp = dst;
	
	for(x = 0; x < len; x+=2)
	{
		dstp[x] = srcp[x + 1];
		
		if( (x+1) < len )
			dstp[x+1] = srcp[x];
	}
}
