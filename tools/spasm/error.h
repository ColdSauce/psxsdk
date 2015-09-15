#ifndef _SPASM_ERROR_H
#define _SPASM_ERROR_H

void instruction_error(char *format, ...);
void instruction_warning(char *format, ...);
void assembler_error(char *format, ...);
extern int yylineno;

#endif

