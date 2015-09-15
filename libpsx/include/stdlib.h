/*
 * stdlib.h
 *
 * Standard library functions
 *
 * PSXSDK
 */

#ifndef _STDLIB_H
#define _STDLIB_H

typedef unsigned int size_t;
typedef signed int ssize_t;

/* Conversion functions */

int atoi(const char *s);
long atol(const char *s);
char *itoa(int value, char *str, int base);
char *ltoa(long value, char *str, int base);
char *lltoa(long long value, char *str, int base);
char *utoa(unsigned int value, char *str, int base);
char *ultoa(unsigned long value, char *str, int base);
char *ulltoa(unsigned long long value, char *str, int base);
//extern char atob(char *s); // Is this right?


// Random number functions

#define RAND_MAX		0x7fffffff

int rand(void);
void srand(unsigned int seed);

// Quick sort

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

// Memory allocation functions

//#warning "malloc() family of functions NEEDS MORE TESTING"

void *malloc(size_t size);
void free(void *buf);
void *calloc(size_t number, size_t size);
void *realloc(void *buf , size_t n);

int abs(int x);
long long strtoll(const char *nptr, char **endptr, int base);
long strtol(const char *nptr, char **endptr, int base);
double strtod(const char *nptr, char **endptr);
long double strtold(const char *nptr, char **endptr);
float strtof(const char *nptr, char **endptr);

// Misc
void abort(void);
void exit(int status);
void call_atexit_callbacks(void);

// Program return codes

#define EXIT_SUCCESS	0
#define EXIT_FAILURE	1

#endif

