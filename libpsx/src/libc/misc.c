#include <stdio.h>
#include <stdlib.h>

static unsigned int rand_seed = 0;
static unsigned int rand_next = 0;

int abs(int x)
{
	if(x<0)return -x;

	return x;
}

void srand(unsigned int seed)
{
	rand_seed = seed;
}

int rand()
{
	rand_next = (rand_next ^ 0x98765432)*0x1357;
	
	return rand_next % RAND_MAX;
}

static char *__ulltoa_internal__(unsigned long long value, char *str, int base, int minus_sign,
	unsigned long long maxp )
{
	unsigned long long p;
	unsigned long long p3;
	int c;
	int a;
	
	p = 1;
	
	do
	{
		p3 = p;
		p *= base;
		
		if(maxp && p > maxp)
			break;
	}while((p >= p3) && !(p % p3));
	
	if(minus_sign)
		*(str++) = '-';
	
	for(a = 0;p3 > 0;p3/=base)
	{
		c = value / p3;
		value %= p3;
		
		if(c)
			a = 1;
		
		if(a)
		{
			if(c <= 9)
				c += '0';
			else
				c = (c - 10) + 'A';
			
			*(str++) = c;
		}
	}
	
	*str = '\0';
	
	return str;
}

char *ulltoa(unsigned long long value, char *str, int base)
{
	return __ulltoa_internal__(value, str, base, 0, 0);
}

char *ultoa(unsigned long value, char *str, int base)
{
	return __ulltoa_internal__(value, str, base, 0, (sizeof(long)==8)?0:0xFFFFFFFF);
}

char *utoa(unsigned int value, char *str, int base)
{
	return __ulltoa_internal__(value, str, base, 0, 0xFFFFFFFF);
}

char *lltoa(long long value, char *str, int base)
{
	return __ulltoa_internal__((value<0)?-value:value, str, base, value<0, 0);
}
	
char *ltoa(long value, char *str, int base)
{
	return __ulltoa_internal__((value<0)?-value:value, str, base, value<0, (sizeof(long)==8)?0:0xFFFFFFFF);
}

char *itoa(int value, char *str, int base)
{
	return __ulltoa_internal__((value<0)?-value:value, str, base, value<0, 0xFFFFFFFF);
}

void abort(void)
{
	printf("abort(): Abnormal program termination\n");
	exit(1);
}
