/* 
 * strings.h
 *
 * PSXSDK
 */

#ifndef _STRINGS_H
#define _STRINGS_H

#include <string.h>
#include <types.h>

#define bcopy(src,dst,len)				memmove(dst,src,len)
#define bzero(ptr, len)					memset(ptr, 0, len)
#define bcmp(b1,b2,len)					memcmp(b1,b2,len)
#define index(s, c)						strchr(s, c)
#define rindex(s, c)						strrchr(s, c)

int ffs(int value);
char *index(const char *s, int c);
char *rindex(const char *s, int c);

/**
 * Returns the number of bits set in the value
 * @param value Value
 * @return Number of bits set in value
 */

unsigned int popcount(unsigned int value);
unsigned int popcountl(unsigned long value);
unsigned int popcountll(unsigned long long value);
unsigned int popcount32(uint32_t value);
unsigned int popcount64(uint64_t value);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t len);

#endif
