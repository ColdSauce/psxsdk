/**
 * PSXSDK
 *
 * bitstring.h
 *
 * Implementation of bitstring.h, a family of macros found in *BSD
 * to manipulate bit strings
 */

#ifndef _BITSTRING_H
#define _BITSTRING_H

#include <stdlib.h>

typedef unsigned char bitstr_t;

#define bitstr_size(nbits) \
			( (nbits + 7) >> 3 )

#define bit_alloc(nbits)	\
		calloc( bitstr_size(nbits) , 1)

#define bit_decl(name, nbits) \
		name[ bitstr_size(nbits) ]

#define bit_clear(name, bit) \
		name[ bit >> 3 ] &= ~(1 << (bit & 7) )
		
#define bit_set(name, bit) \
		name[ bit >> 3] |= 1 << (bit & 7)
		
#define bit_nclear(name, start, stop) \
		do{ \
			int _b__;\
			for(_b__ = start; _b__ <= stop; _b__++) \
				bit_clear(name, _b__);\
		}while(0)

#define bit_nset(name, start, stop) \
		do{ \
			int _b__;\
			for(_b__ = start; _b__ <= stop; _b__++) \
				bit_set(name, _b__); \
		}while(0)
			
#define bit_test(name, bit) \
		( name[bit >> 3] & (1 << (bit & 7) ) )
		
#define bit_ffs(name, nbits, value) \
		do{\
			int _b__;\
			for(_b__ = 0; _b__ < nbits; _b__++) \
			{\
				printf("bb = %d\n", _b__);\
				if( bit_test(name, _b__) ) \
				{\
					*(value) = _b__; \
					break; \
				}\
			}\
			if( _b__ == nbits ) *(value) = -1;\
		}while(0)

#define bit_ffc(name, nbits, value) \
		do{\
			int _b__;\
			for(_b__ = 0; _b__ < nbits; _b__++) \
			{\
				if( !bit_test(name, _b__) ) \
				{\
					*(value) = _b__; \
					break; \
				}\
			}\
			if( _b__ == nbits ) *(value) = -1;\
		}while(0)
		
#endif
