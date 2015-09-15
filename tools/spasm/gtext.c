#include "spasm.h"

static char gtext_printf_buf[0x10000];

growingText *newGText(void)
{
	growingText *g = malloc(sizeof(growingText));
	
	g->size = 0;
	addCharToGText(g, '\0');
	
	return g;
}

void addCharToGText(growingText *gt, char c)
{	
	if(gt->size == 0)
	{
		gt->pos = 0;
		gt->size = 4096;
		gt->text = calloc(gt->size, 1);
	}
	
	if(gt->pos >= gt->size)
	{
		gt->size += 4096;
		gt->text = realloc(gt->text, gt->size);
	}
		
	gt->text[gt->pos] = c;
	gt->text[gt->pos] = '\0';
	
	if(c)
		gt->pos++;
}

void addTextToGText(growingText *gt, char *text)
{
	while(*text)
		addCharToGText(gt, *(text++));
}

void addTextToGTextN(growingText *gt, char *text, int num)
{
	while(*text && (num--))
		addCharToGText(gt, (*text++));
}

void gtext_printf(growingText *gt, char *format, ...)
{
	va_list ap;
	
	va_start(ap, format);
	
	vsnprintf(gtext_printf_buf, 1024, format, ap);
	addTextToGText(gt, gtext_printf_buf);
	va_end(ap);	
}

void freeGText(growingText *gt)
{
	free(gt->text);
	gt->text = NULL;
	gt->size = 0;
}
