#ifndef _SPASM_PARSER_H
#define _SPASM_PARSER_H

enum
{
	T_INTEGER = 1, T_REGISTER = 2, T_LABEL = 3
};

extern char *fileBuffer;
extern unsigned int fileBufferSize;

extern char *curText;
extern char *curLine;

unsigned int asm_atoi(char *arg);
char *spasm_parser(char *text, int pass);

extern char curIns[128];
extern unsigned int curInsArg;
extern unsigned int curInsArgT;
extern unsigned int insArgv[64];
extern unsigned int insArgt[64];
extern unsigned int insArgc;
extern int rawArgc;
extern char rawArgv[64][128];
extern FILE *asmOut;
extern int line_number;
extern int atoiT[64];
extern int inMacro, macroArgc;
extern char macroArgv[16][256];
#endif
