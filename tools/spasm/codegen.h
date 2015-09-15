#ifndef _SPASM_CODEGEN_H
#define _SPASM_CODEGEN_H

typedef struct
{
	char name[128];
	unsigned int pc;
	unsigned int pass;
}asm_label;

typedef struct
{
	char name[128];
	int argc;
	char *text;
}asm_macro;

extern asm_label *labels;
extern asm_macro *macros;

extern volatile unsigned int curPc;
extern int curPass;
extern unsigned int numLabels;
extern unsigned int startAddress;
extern unsigned int copn;
extern int first_instruction;
extern int org_found;
extern void (*INSFUNC)(void);

void codegen_init(void);
void add_label(char *label, unsigned int pc);
void add_label_equ(char *label, unsigned int pc);
unsigned int find_label(char *label);
void find_label_reset();
int find_label_ok();
void add_macro(char *name, char *text, int argc);
asm_macro *find_macro(char *name);
growingText *expand_macro(asm_macro *macro, char **argv);
int label_was_not_found_once(char *name);
void add_not_found_label(char *name);
//void resolve_labels();


#endif
