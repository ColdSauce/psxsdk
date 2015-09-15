/* stdint.h */

#ifndef _STDINT_H
#define _STDINT_H

#include <inttypes.h>

// Added ifndef or otherwise GCC 4.8 would complain

#ifndef __PTRDIFF_TYPE__
typedef unsigned int ptrdiff_t;
#endif

#endif
