/*
 * string.h
 *
 * Prototypes for string functions of the C library
 *
 * PSXSDK
 */

// NOTE: The BIOS was found to be unreliable for many functions,
// so it is not used anymore for the libc.

#ifndef _STRING_H
#define _STRING_H

#include <strings.h> // for backwards compatibility
#include <types.h>

char *strcat(char *s , const char *append);
char *strncat(char *s , const char *append, size_t n);
int strcmp(const char *dst , const char *src);
int strncmp(const char *dst , const char *src , size_t len);
int stricmp(const char *s1, const char *s2); // alias of strcasecmp
int strnicmp(const char *s1, const char *s2, size_t len); // alias of strncasecmp
char *strcpy(char *dst , const char *src);
char *strncpy(char *dst , const char *src , size_t n);
int strlen(const char *s);
int strnlen(const char *s, size_t maxlen);
char *strchr(const char *s , int c);
char *strrchr(const char *s , int c);
char *strpbrk(const char *dst , const char *src);
int strspn(const char *s , const char *charset);
int strcspn(const char *s , const char *charset);
char *strsep(char **stringp, const char *delim);
char *strtok(char *str, const char *sep);
char *strstr(const char *big , const char *little);
char *strcasestr(const char *big, const char *little);
char *strlwr(char *string);
char *strupr(char *string);
char *strdup(const char *str);
char *strndup(const char *str, size_t len);
int strlcpy(char *dst, const char *src, size_t size);
int strlcat(char *dst, const char *src, size_t size);

void *memset(void *dst , char c , size_t n);
void *memmove(void *dst , const void *src , size_t n);
int memcmp(const void *b1 , const void *b2 , size_t n);
void *memchr(void *s , int c , size_t n);
void *memrchr(void *b, int c, size_t len);
void *memcpy(void *dst , const void *src , size_t len);
void *memccpy(void *dst, const void *src, int c, size_t len);
void *memmem(const void *big, size_t big_len, const void *little, size_t little_len);

// ffsl() and ffsll() are glibc extensions, and are in string.h
// instead of strings.h (like ffs()) for some reason..

int ffsl(long value);
int ffsll(long long value);

#endif

