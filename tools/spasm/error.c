#include "spasm.h"

static void show_line(void)
{
	printf("%s\n", curLine);
	printf("^^^^^^^^^^^^^^\n");
}
	
void instruction_error(char *format, ...)
{
	va_list ap;
	
	va_start(ap, format);
	
	printf("Line %d: Error(%s) - ", line_number, curIns);
	vprintf(format, ap);
	printf("\n");
	show_line();

	va_end(ap);
	
	exit(EXIT_FAILURE);
}

void instruction_warning(char *format, ...)
{
	if(curPass <= 0)
		return;
	
	va_list ap;
	
	va_start(ap, format);
	
	printf("Line %d: Warning (%s) - ", line_number, curIns);
	vprintf(format, ap);
	printf("\n");
	show_line();
	
	va_end(ap);
}

void assembler_error(char *format, ...)
{
	va_list ap;
	
	va_start(ap, format);

	printf("Line %d, assembler error: ", line_number);
	vprintf(format, ap);
	printf("\n");
	show_line();
	
	va_end(ap);
	
	exit(EXIT_FAILURE);
}

	