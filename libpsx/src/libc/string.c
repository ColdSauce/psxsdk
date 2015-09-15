/*
 * string.c
 * 
 * Part of the PSXSDK C library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

void *memcpy(void *dst, const void *src, size_t len)
{
	void *dst2 = dst;
	
	while(len--)
		*(((unsigned char*)dst++)) = *(((unsigned char*)src++));
		
	return dst2;
}

void *memccpy(void *dst, const void *src, int c, size_t len)
{	
	unsigned char c2;
	
	while(len--)
	{
		*(((unsigned char*)dst++)) = ( c2 = *(((unsigned char*)src++)) );
		
		if(c2 == c)
			return (void*)src;
	}
	
	return NULL;
}

void *memset(void *dst , char c , size_t n)
{
	unsigned char *dstc = (unsigned char*)dst;
	int x;

	for(x = 0; x < n; x++)
		dstc[x] = c;
	
	return dst;
}

int memcmp(const void *b1, const void *b2, size_t n)
{
	int x;
	unsigned char *bp1 = (unsigned char*)b1;
	unsigned char *bp2 = (unsigned char*)b2;
	
	for(x = 0;  x < n; x++)
		if(bp1[x] != bp2[x])
			return (bp1[x] - bp2[x]);
		
	return 0;
}

void *memmove(void *dst, const void *src, size_t len)
{
	void *dst2 = dst;
	
	dst+=len-1;
	src+=len-1;
	
	while(len--)
		*(((unsigned char*)dst--)) = *(((unsigned char*)src--));
		
	return dst2;
}

void *memchr(void *s , int c , size_t n)
{
	while(n--)
	{
		if(*((unsigned char*)s) == (unsigned char)c)
			return s;
		
		s++;
	}
	
	return NULL;
}

char *strncpy(char *dst, const char *src, size_t len)
{
	char *odst=dst;

	while(*src && len)
	{
		*(dst++) = *(src++);
		len--;
	}
	
	if(len)*dst = 0;
	
	return odst;
}

char *strcpy(char *dst, const char *src)
{
	char *odst = dst;

	while((*(dst++) = *(src++)));
	return odst;
}

int strlen(const char *str)
{
	int i = 0;
	while(*(str++))i++;
	return i;
}

char *strchr(const char *s, int c)
{
	int x;
	int l = strlen(s);

	for(x = 0; x <= l; x++)
		if(s[x] == c) return (char*)&s[x];

	return NULL;
}

char *strrchr(const char *s, int c)
{
	int x;
	int l = strlen(s);

	for(x = l; x>=0; x--)
		if(s[x] == c) return (char*)&s[x];

	return NULL;
}

char *strpbrk(const char *s, const char *charset)
{
	int x,y;

	for(x = 0; s[x] != 0; x++)
		for(y = 0; charset[y] != 0; y++)
			if(s[x] == charset[y]) return (char*)&s[x];

	return NULL;
}

char *strstr(const char *big, const char *little)
{
	int ls = strlen(little);
	int bs = strlen(big);
	int x;

	if(ls == 0)
		return (char*)big;
	
	if(ls > bs)
		return NULL;

	for(x = 0; x <= bs-ls; x++)
		if(memcmp(little, &big[x], ls) == 0)
			return (char*)&big[x];

	return NULL;
}

int strcmp(const char *s1, const char *s2)
{
	while(*s1 && *s2 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}

	return *s1-*s2;
}

int strncmp(const char *s1, const char *s2, size_t len)
{
	int p = 0;

	while(*s1 && *s2 && (*s1 == *s2) && p<len)
	{
		p++;

		if(p<len)
		{
			s1++;
			s2++;
		}
	}

	return *s1-*s2;
}

char *strdup(const char *str)
{
	char *ns = (void*)malloc(strlen(str) + 1);

	if(ns == NULL)
		return NULL;
	
	strcpy(ns, str);
	return ns;
}

char *strndup(const char *str, size_t len)
{
	int n=strlen(str);
	char *ns = (void*)malloc((n+1)>len?len:(n+1));

	if(ns == NULL)
		return NULL;
	
	strncpy(ns, str, (n+1)>len?len:(n+1));
	return ns;
}
	
long long strtoll(const char *nptr, char **endptr, int base)
{
	int r = 0;
	int t = 0;
	int n = 0;
	
	while(*nptr && isspace(*nptr))
		nptr++;
	
	if(*nptr == '-')
	{
		nptr++;
		n = 1;
	}

	if(base == 0)
	{
		if(*nptr == '0')
			base = 8;
		else
			base = 10;
	}

	if(!(base >= 2 && base <= 36))
		return 0;

	if(base == 16 && *nptr == '0')
	{
		if(*(nptr+1) == 'x' || *(nptr+1) == 'X')
			nptr+=2;
	}

	while(*nptr)
	{
		switch(*nptr)
		{
			case '0'...'9':
				t = *nptr - '0';
			break;
			case 'a' ... 'z':
				t = (*nptr - 'a') + 10;
			break;
			case 'A' ... 'Z':
				t = (*nptr - 'A') + 10;
			break;
			default:
				t = 1000;
			break;
		}

		if(t>=base)
			break;

		r*=base;
		r+=t;
		nptr++;
	}

	if(endptr)*endptr = (char*)nptr;
	return n?-r:r;
}

long strtol(const char *nptr, char **endptr, int base)
{
	return (long)strtoll(nptr, endptr, base);
}

double strtod(const char *nptr, char **endptr)
{
	char strbuf[64];
	int x = 0;
	int y;
	double i=0, d=0;
	int s=1;

	if(*nptr == '-')
	{
		nptr++;
		s=-1;
	}

	while(*nptr >= '0' && *nptr <= '9' && x < 18)
		strbuf[x++] = *(nptr++);

	strbuf[x] = 0;

	i = (double)strtoll(strbuf, NULL, 10);

	if(*nptr == '.')
	{
		nptr++;
		x = 0;

		while(*nptr >= '0' && *nptr <= '9' && x < 7)
			strbuf[x++] = *(nptr++);

		strbuf[x] = 0;
		
		if(endptr != NULL) *endptr = (char*)nptr;

		y=1;

		for(x=0;strbuf[x]!=0;x++)
			y*=10;

		d = (double)strtoll(strbuf, NULL, 10);
		d /= y;
	}
	else
	{
		if(endptr != NULL)
			*endptr = (char*)nptr;
	}
	
	return (i + d)*s;
}

long double strtold(const char *nptr, char **endptr)
{
	return (long double)strtod(nptr, endptr);
}

float strtof(const char *nptr, char **endptr)
{
	return (float)strtod(nptr, endptr);
}

char *strcat(char *s, const char *append)
{
	strcpy(&s[strlen(s)], append);

	return s;
}

char *strncat(char *s, const char *append, size_t count)
{
	strncpy(&s[strlen(s)], append, count);

	return s;
}

int strcasecmp(const char *s1, const char *s2)
{
	while(tolower(*s1) && tolower(*s2) && (tolower(*s1) == tolower(*s2)))
	{
		s1++;
		s2++;
	}

	return tolower(*s1)-tolower(*s2);
}

int strncasecmp(const char *s1, const char *s2, size_t len)
{
	int p = 0;

	while(tolower(*s1) && tolower(*s2) && (tolower(*s1) == tolower(*s2)) && p<len)
	{
		p++;

		if(p<len)
		{
			s1++;
			s2++;
		}
	}

	return tolower(*s1)-tolower(*s2);
}

int stricmp(const char *s1, const char *s2)
{
	return strcasecmp(s1,s2);
}

int strnicmp(const char *s1, const char *s2, size_t len)
{
	return strncasecmp(s1, s2, len);
}

//static char *strtok_string;

char *strsep(char **stringp, const char *delim)
{
	//int x,y;
	char *old = *stringp;
	const char *s;
	int ok = 0;
		
	while(**stringp && !ok)
	{
		s = delim;
		
		while(*delim)
		{
			if(**stringp == *delim)
			{
				**stringp = 0;
				ok = 1;
				break;
			}
			
			delim++;
		}
		
		delim = s;
		
		*stringp+=1;
	}
	
	if(!ok)*stringp = NULL;
	
	return old;
}

char *strtok(char *str, const char *sep)
{
	int x, y;
	static char *strtok_string;
	static int strtok_len;
	static int strtok_pos;
	//int strtok_oldpos = 0;
	
	if(str != NULL)
	{
		strtok_string = str;
		strtok_len = strlen(str);
		
		for(x = 0; x < strtok_len; x++)
		{
			for(y = 0; sep[y] != 0; y++)
			{
				if(strtok_string[x] == sep[y])
				{
					strtok_string[x] = 0;
					break;
				}
			}
		}
		
		strtok_pos = 0;
		
		while(strtok_pos < strtok_len)
		{
			if(strtok_string[strtok_pos])
				return &strtok_string[strtok_pos];
		
			strtok_pos++;
		}
	}
	
	while(strtok_pos < strtok_len)
	{
		if(!strtok_string[strtok_pos])
		{
			strtok_pos++;
			break;
		}
		
		strtok_pos++;
	}
	
	while(strtok_pos < strtok_len)
	{

		if(strtok_string[strtok_pos])
			return &strtok_string[strtok_pos];
		
		strtok_pos++;
	}
		
	return NULL;
}

int strspn(const char *s, const char *charset)
{
	int x, y;
	int appears;
	
	for(x = 0; s[x] != 0; x++)
	{		
		appears = 0;
		
		for(y = 0; charset[y] != 0; y++)
		{
			if(s[x] == charset[y])
			{
				appears = 1;
				break;
			}
		}
		
		if(!appears)break;
	}
	
	return x;
}

int strcspn(const char *s, const char *charset)
{
	int x, y;
	int appears;
	
	for(x = 0; s[x] != 0; x++)
	{		
		appears = 0;
		
		for(y = 0; charset[y] != 0; y++)
		{
			if(s[x] == charset[y])
			{
				appears = 1;
				break;
			}
		}
		
		if(appears)break;
	}
	
	return x;
}

char *strlwr(char *string)
{
	char *old = string;
	
	while(*string)
	{
		*string = tolower(*string);
		string++;
	}
	
	return old;
}

char *strupr(char *string)
{
	char *old = string;
	
	while(*string)
	{
		*string = toupper(*string);
		string++;
	}
	
	return old;
}

int atoi(const char *string)
{
	return (int)strtol(string, NULL, 10);
}

long atol(const char *string)
{
	return strtol(string, NULL, 10);
}

int strnlen(const char *s, size_t maxlen)
{
	int l=0;
	
	while(*(s++) && l<maxlen)
		l++;
	
	return l;
}

void *memrchr(void *b, int c, size_t len)
{
	int i = len - 1;
	unsigned char *p = b;
	
	for(i = len - 1; p[i] != (unsigned char)c && i >= 0;i--);
	
	return (i>=0)?&p[i]:NULL;
}

char *stpcpy(char *dst, const char *src)
{
	do
	{
		*(dst++) = *src;
	}while(*(src++));
	
	return dst-1;
}

char *stpncpy(char *dst, const char *src, int len)
{
	int c = 0;
	
	do
	{
		if(c < len)
			*(dst++) = *src;
		
		c++;
	}while(*(src++) && c < len);
	
	return dst-1;
}

char *strcasestr(const char *big, const char *little)
{
	while(*big)
	{
		const char *pbig = big;
		const char *plittle = little;
		int ok = 1;
		
		while(*pbig)
		{
			if(tolower(*pbig) != tolower(*plittle))
			{
				ok = 0;
				break;
			}
			
			pbig++;
			plittle++;
		}
		
		if(ok)
			return (char*)big;
		
		big++;
	}
	
	return NULL;
}

int strlcpy(char *dst, const char *src, size_t size)
{
	char *src_end = memchr((void*)src, '\0', size);
	
	if(src_end == NULL)
		return 0;

	memcpy(dst, src, src_end - src);
	
	return (src_end - src);
}

int strlcat(char *dst, const char *src, size_t size)
{
	int dstl = strlen(dst);
	char *q = dst + dstl;
	int real_size = size;
	
	if(memchr((void*)src, '\0', size))
		real_size = strlen(src);
	
	memcpy(q, src, real_size-dstl-1);
	if(real_size != size) q[real_size-dstl-1] = '\0';
	
	return size-dstl;
}

void *memmem(const void *big, size_t big_len, const void *little,
	size_t little_len)
{
	int i, j, l;
	unsigned char *bigp = (unsigned char*)big;
	unsigned char *littlep = (unsigned char*)little;
	
	for(i = 0, l = (int)(big_len - little_len); i <= l; i++, bigp++)
	{
		for(j = 0; j < little_len; j++)
		{
			if(littlep[j] != bigp[j])
				break;
		}
		
		if(j == little_len)
			return bigp;
	}
	
	return NULL;
}
