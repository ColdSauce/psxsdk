#include <stdlib.h>
#include <strings.h>
#include <stdint.h>

#define ffs_func(func_name, type) \
\
int func_name(type value) \
{ \
	int i; \
	int nbits = sizeof(type) * 8;\
	\
	for(i = 0; i < nbits && !(value & ((type)1 << i) ); i++);\
	\
	return (i == nbits) ? 0 : (i + 1);\
}

ffs_func(ffs, int);
ffs_func(ffsl, long);
ffs_func(ffsll, long long);

#define popcount_func(func_name, type)	\
\
unsigned int func_name(type value) \
{ \
	int i, bitcnt; \
	int nbits = sizeof(type) * 8; \
	\
	for(i = 0, bitcnt = 0; i < nbits; i++) \
	{\
		if( value & ((type)1 << i) )\
			bitcnt++;\
	}\
	\
	return bitcnt;\
}

popcount_func(popcount, unsigned int);
popcount_func(popcountl, unsigned long);
popcount_func(popcountll, unsigned long long);
popcount_func(popcount32, uint32_t);
popcount_func(popcount64, uint64_t);
