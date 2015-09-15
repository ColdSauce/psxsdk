#include "spasm.h"

char curIns[128];
unsigned int curInsArg;
unsigned int curInsArgT;
unsigned int insArgv[64];
unsigned int insArgc;
unsigned int insArgt[64];
unsigned int copn;
int org_found;

void (*INSFUNC)(void);

volatile unsigned int curPc = 0;
int curPass = 0;
unsigned int startAddress = 0;
unsigned int numLabels;
unsigned int numLabelsAlloc;
unsigned int numMacros;
unsigned int numMacrosAlloc;
int first_instruction;
asm_label *labels;
asm_macro *macros;
static int find_label_status = 1;

void codegen_init(void)
{
	curPc = startAddress;
	curPass = 0;
	numLabels = 0;
	numLabelsAlloc = 0;
	numMacros = 0;
	numMacrosAlloc = 0;
	labels = NULL;
	macros = NULL;
}
		
static asm_label *find_label_internal(char *name)
{
	int i;
	
	for(i = 0; i < numLabels; i++)
	{
		if(strcmp(name, labels[i].name) == 0)
			return &labels[i];
	}
	
	return NULL;
}

static void add_label_internal(char *name, unsigned int pc)
{
	// add labels only if current pass >= 1!
	asm_label *l;
	
/*	if(curPass == )
		return;
	
	if(curPass >= 2)
		return;*/

	//printf("Name = %s\n", name);
	
	l = find_label_internal(name);	
		
	if(l)
	{
		if(l->pc != pc)
		{
			//if(l->pass == curPass)
			//	assembler_error("Impossible to redefine label %s", name);
			
			//printf("Redefining, [%s] = %08X, pass %d\n", l->name, pc, curPass);
			l->pc = pc;
		}
				
		return;
	}
	
	if(numLabels == numLabelsAlloc)
	{
		numLabelsAlloc += 128;
		labels = realloc(labels, sizeof(asm_label) * numLabelsAlloc);
	}
	
	strncpy(labels[numLabels].name, name, 127);
	labels[numLabels].pass = curPass;
	
	labels[numLabels].pc = pc;
		
	numLabels++;
	
	//printf("label #%d, [%s] = %08X, pass = %d\n", numLabels, name, pc, curPass);
	
	/*while(*name)
	{
		printf("%x, \'%c\'\n", *name, *name);
		name++;
	}*/
}

	

void add_label(char *name, unsigned int pc)
{
	if(curPass == -1)
		return;
	
	return add_label_internal(name, pc);
}

void add_label_equ(char *name, unsigned int pc)
{
	return add_label_internal(name, pc);
}

unsigned int find_label(char *name)
{
	//printf("find_label(%s)\n", name);
	
	asm_label *l = find_label_internal(name);
	
	if(l)
	{
		//find_label_status = 1;
		return l->pc;
	}
	
// remember! if pass >= 1, abort if you can't find a label, because that means it was really
// impossible to find.	
	
//	printf(">>DEBUG, PASS = %d << Couldn't find label %s$\n",  curPass, name);
		
	find_label_status = 0;
	
	if(curPass == 1)
		instruction_error("Cannot find label %s", name);
	
	return 0xFFFFFFFF;
}

void find_label_reset()
{
	find_label_status = 1;
}

int find_label_ok()
{
	return find_label_status;
}

void add_macro(char *name, char *text, int argc)
{
	if(find_macro(name))
		assembler_error("Macro %s already defined", name);
	
	if(numMacros == numMacrosAlloc)
	{
		numMacrosAlloc += 128;
		macros = realloc(macros, sizeof(asm_macro) * numMacrosAlloc);
	}
	
	strncpy(macros[numMacros].name, name, 127);
	macros[numMacros].text = strdup(text);
	macros[numMacros].argc = argc;
	
	numMacros++;
}

asm_macro *find_macro(char *name)
{
	int i;
	
	for(i = 0; i < numMacros; i++)
	{
		if(strcmp(name, macros[i].name) == 0)
			return &macros[i];
	}
	
	return NULL;
}

growingText *expand_macro(asm_macro *macro, char **argv)
{	
	growingText *gt = newGText();
	char *mt = macro->text;
	
	while(*mt)
	{
		if(*mt == 0x7F && *(mt+1) == 0x7F)
		{			
			int n;
			
			sscanf(mt+2, "%d", &n);
			
			addTextToGText(gt, argv[n]);
			
			mt+=6;
		}
		else
			addCharToGText(gt, *(mt++));
	}
	
	return gt;
}

/*void arg_push()
{	
	if(curInsArgT == 0xFF)
		return;
	
	if(insArgc <  4)
	{
		insArgv[insArgc] = curInsArg;
		insArgt[insArgc] = curInsArgT;
	}
		
	//printf("ARG(%d) = %d, %08x (%d)\n", insArgc, insArgv[insArgc],
	//	insArgv[insArgc], insArgt[insArgc]);
	
	curInsArgT = 0xFF;
	
	insArgc++;
}*/

	
	