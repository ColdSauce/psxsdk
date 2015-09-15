#ifndef _GTEXT_H
#define _GTEXT_H

typedef struct
{
	int size;
	int pos;
	char *text;
}growingText;

growingText *newGText(void);
void addCharToGText(growingText *gt, char c);
void addTextToGText(growingText *gt, char *text);
void addTextToGTextN(growingText *gt, char *text, int num);
void gtext_printf(growingText *gt, char *format, ...);
void freeGText(growingText *gt);

#endif
