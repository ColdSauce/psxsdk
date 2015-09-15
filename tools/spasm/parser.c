#include "spasm.h"

int atoiT[64];
int inMacro, macroArgc;
char macroName[128];
char macroArgv[16][256];
int macroActualStartI, macroStartI, macroEndI, macroActualEndI;

/*
 * There is preliminary macro support, but it is not finished yet,
 * and so it is disabled.
 * Have fun hacking.
 */

static char *reg_names[] =
{
	"zero", 
	"at",
	"v0", "v1",
	"a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9",
	"k0", "k1",
	"gp", "sp",
	"fp",
	"ra",
	NULL
};

//static void *THIS_IS_A_MACRO = (void*)1;


/*static void addCharToText(growingText *gt, char c)
{
	char s[2] = {c, '\0'};
	addText(gt, s);
}*/


static int regtoi(char *arg)
{
	int i;

	if(strcmp(arg, "s8") == 0)
		arg = "fp";
	
	for(i = 0; reg_names[i]; i++)
	{
		if(strcmp(arg, reg_names[i]) == 0)
			return i;
	}
	
	return -1;
}

unsigned int asm_atoi(char *arg)
{    
	unsigned int i;
		
    	
	if((i = regtoi(arg)) != -1)
	{
		atoiT[insArgc] = T_REGISTER;
		return i;
	}
	else if(tolower((int)*arg) == 'r' &&( (strlen(arg) == 2 && isdigit((int)*(arg+1))) || 
		(strlen(arg) == 3 && isdigit((int)*(arg+1)) && isdigit((int)*(arg+2)))))
	{
		sscanf(arg+1, "%d", &i);
		atoiT[insArgc] = T_REGISTER;
		return i;
	}
	else if(*arg == '-' && *(arg+1) == '$' && isxdigit((unsigned int)*(arg+2)) )
	{
		sscanf(arg+2, "%x", &i);
		atoiT[insArgc] = T_INTEGER;
		return -i;
	}
	else if(*arg == '$' )
	{
		sscanf(arg+1, "%x", &i);
		atoiT[insArgc] = T_INTEGER;

		return i;
	}
	else if(strcmp(arg, "*") == 0)
	{
		atoiT[insArgc] = T_INTEGER;
		
		return curPc;
	}
	else if(isalpha((unsigned int)*arg) || (*arg) == '_')
	{
		atoiT[insArgc] = T_LABEL;
		
		i = find_label(arg);
		
		return i;
	}
	
	sscanf(arg, "%i", &i);
	atoiT[insArgc] = T_INTEGER;
	
	return i;
}
		
enum
{
	INITIAL, ARG_ENTER, COMMENT
};

/*static unsigned int is_spasm_sep_char(char c)
{
	return (c == '-' ||
		c == '+' ||
		c == '&' ||
		c == '|' ||
		c == '!' ||
		c == '*' ||
		c == '(' ||
		c == ')' ||
		c == '>' ||
		c == '<' ||
		c == '\n' ||
		c == '\r' ||
		c == '\t' ||
		c == ',' ||
		c == ';' ||
		c == ' ' ||
		c == '\0');
}*/

char *spasm_parser(char *text, int pass)
{
	int i, oldI, j, l, m;
	char linebuf[1024];
	char linebuf2[1024];
	char linebuf3[1024];
	char argbuf[1024];
	char *tok[256];
	int state = INITIAL;
	int num_of_tok=0;
	char *t;
	curText = text;
	unsigned int v;
	
/*	int k;
	int macroState;
	int num_of_macrotok=0;
	char *macroText;
	asm_macro *curMacro;
	growingText macroContent = {0, 0, NULL};
	char macroLinebuf[1024];
	char *macrotok[256];
*/

	//FILE *logFile;

	//if(curPass == 0)
	//	logFile = fopen("log", "wb");

	for(i = 0; i < 16; i++)
		macroArgv[i][255] = 0;
	
theBeginning:
	i = 0;
	inMacro = 0;
	curPass = pass;
	org_found = 0;
	first_instruction = 1;
	line_number = 0;
	text = curText;
	
	while(text[i])
	{
		state = INITIAL;

		oldI = i;
		
		for(j = 0; text[i] && text[i] != '\n'; i++)
		{
			if(j < 1023 && text[i] != '\r')
				linebuf[j++] = text[i];
		}
		
		line_number++;
		rawArgc = insArgc = 0;
		INSFUNC = NULL;
		
		if(text[i] == '\n')
			i++;
		
		linebuf[j] = '\0';

//tokenize_line:				
		strcpy(linebuf2, linebuf); // Keep a second copy, we will need it later.
		strcpy(linebuf3, linebuf);
		//if(curPass==1)printf("linebuf[%d] %d:= %s\n",  curPass, line_number, linebuf);
		curLine = linebuf3;

		char *a = linebuf;
		char *s;
		j = 0;
	
		num_of_tok = 0;
		
		for(m = 0; m < 256; m++)
			tok[m]=NULL;
		
		while((s = strtok(a, " \t")))
		{
			tok[num_of_tok++] = s;
			a = NULL;
		}
		
		tok[num_of_tok] = NULL;
		
		j = 0;
		

		
		while((s = tok[j]))
		{
			//printf("tok[%d] = %s\n", j, tok[j]);
			
			find_label_reset();
			
			if(strlen(s) == 0)
			{ // A token with zero length is garbage, skip it
				j++;
				continue;
			}
				
			//printf("s = ^%s\n", s);
			
			switch(state)
			{
				case INITIAL: // Initial case
					
					// Is this token a comment?
				
					if(*s == ';')
					{
						state = COMMENT;
						break;
					}

					// Is this token an instruction?
				
					if((INSFUNC = get_instruction(s)))
					{
						strncpy(curIns, s, 127);
						s[127] = '\0';
						insArgc = 0;
						state = ARG_ENTER;
						argbuf[0] = '\0';
						
						break;
					}
					
					// Is this token a macro?
					
				/*	if((curMacro = find_macro(s)))
					{
						INSFUNC = THIS_IS_A_MACRO;
						strncpy(curIns, s, 127);
						s[127] = '\0';
						insArgc = 0;
						state = ARG_ENTER;
						argbuf[0] = '\0';
						
						break;
					}
					
					// If we are in a macro definition, check if this is the end of the macro definition
					if(inMacro && strcasecmp(s, "endm") == 0)
					{
						//printf("MACRO ENDS!!\n");
						inMacro = 0;
						macroActualEndI = i;
						macroEndI = oldI;
				
						l = macroEndI - macroStartI;
						macroText = malloc(l+1);
						macroText[l] = '\0';
						strncpy(macroText, &text[macroStartI], l);
						
						for(l = macroActualStartI; l < macroActualEndI-1; l++)
							text[l] = ' ';
						
						text[l] = '\n';
						
						int substLen;
				
						addCharToGText(&macroContent, '\0');
						
						//while(macroText[
						for(l = 0; macroText[l];)
						{
							//printf("l = %d\n", l);
							
							for(k = 0; macroText[l] && macroText[l] != '\n'; l++)
							{
								if(j < 1023 && macroText[l] != '\r')
									macroLinebuf[k++] = macroText[l];
							}
							
							if(macroText[l] == '\n')
								l++;
							
							macroLinebuf[k] = '\0';
							
							int quotesOpen=0;
							int quotesChar=-1;							
							m = k;
														
							for(k = 0; k < m; k++)
							{
								//printf("k = %d\n", k);
								
								if(macroLinebuf[k] == '"' || macroLinebuf[k] == '\'')
								{
									if(!quotesOpen)
									{
										quotesChar = macroLinebuf[k];
										quotesOpen = 1;
									}
									else if(quotesOpen && quotesChar == macroLinebuf[k])
										quotesOpen = 0;
									
									addCharToGText(&macroContent, macroLinebuf[k]);
								}
								else if(!quotesOpen)
								{
									if(macroLinebuf[k] == ';')
									{
										macroLinebuf[k] = '\0';
										break;
									}
									else 
									{
										int it;
										
										for(it = 0; it < macroArgc; it++)
										{
											substLen = strlen(macroArgv[it]);
										
											if(memcmp(&macroLinebuf[k],
												macroArgv[it], substLen) == 0)
											{
												if(is_spasm_sep_char(macroLinebuf[k+substLen]))
												{
													if((k > 0 && is_spasm_sep_char(macroLinebuf[k-1])) || k == 0)
													{
														addCharToGText(&macroContent, 0x7F);
														addCharToGText(&macroContent, 0x7F);
														addCharToGText(&macroContent, (it/1000) + '0');
														addCharToGText(&macroContent, (it%1000)/100 + '0');
														addCharToGText(&macroContent, (it%100)/10 + '0');
														addCharToGText(&macroContent, (it%10) + '0');
												//		addCharToText(&macroContent, ' ');
												//		addCharToText(&macroContent, 0xAA);
												//		addCharToText(&macroContent, 0xCC);
												//		addCharToText(&macroContent, (it+32) & 0xff);
												//		addCharToText(&macroContent, (it>>8)+32);
														k+=substLen;
														break;
													}
												}
											}
										}
										
										if(macroLinebuf[k])
											addCharToGText(&macroContent, macroLinebuf[k]);
									}
								}else if(quotesOpen)
									addCharToGText(&macroContent, macroLinebuf[k]);
							}
							
							addCharToGText(&macroContent, '\n');
						}
						
						add_macro(macroName, macroContent.text, macroArgc);
												
						freeGText(&macroContent);
						free(macroText);
						
			
						
						goto theBeginning;
					}*/
					
					// Now we know it's a label
					// There are two possible cases now
					// - It's a label with a specified value: i.e. label EQU value
					// - It's a label which has the current value of the program counter
					
					// First, we will check for EQU
					
					if((j + 2) < num_of_tok)
					{  // If there are not enough tokens in the line, don't bother checking for EQU.
						if(strcasecmp(tok[j+1], "equ") == 0 || strcasecmp(tok[j+1], "=") == 0)
						{ // EQU found! Set label value to the one specified.
							
							// Set current instruction to EQU
							strcpy(curIns, "equ");
							
							// Remove quotes. Yes, in SPASM, values in EQU statements can have quotes
							// even if they are numerical values!
							
							if( (t = strchr(tok[j+2], '"')) )
							{
								*t = '\0';
								if( (t = strrchr(tok[j+2], '"')) ) 
									*t = '\0';
							}
							
							find_label_reset();
							
							v = spasm_eval(tok[j+2]);
							
							if(strchr(tok[j+2], ';'))
								state = COMMENT;
						
							if(!find_label_ok())
								assembler_error("Can't resolve expression for EQU statement");
							
							if(find_label_ok())
								add_label_equ(tok[j], v);
							else
								add_label(tok[j], v);
							
							j+=2;// As we have processed the EQU, we need to jump two tokens ahead
							break;
						}
					}
					
					//printf("tok[j] = %s\n", tok[j+1]);
					
					/*if((j + 1) < num_of_tok)
					{
					    // We will check if this is a macro
						if(strcasecmp(tok[j+1], "macro") == 0)
						{
							
							strncpy(macroName, tok[j], 127);
							macroName[127] = '\0';
							macroArgc = 0;
							
							if(tok[j+2])
							{
								
								if((t = strchr(tok[j+2], ';')))
								{
									*t = '\0';
									state = COMMENT;
								}
								
								for(t = strtok(tok[j+2], ",") ; t != NULL && macroArgc < 16
									; t = strtok(NULL, ","))
									strncpy(macroArgv[macroArgc++], t, 255);
							}
							
							inMacro = 1;
							
							macroActualStartI = oldI;
							macroStartI = i;
							
							if(tok[j+2])j++;
							j++;
							break;
						}
					}*/
					
					// At this point, it is a label which has the current value of the program counter
					
					if((t = strrchr(tok[j], ':')))
						*t='\0'; // Remove trailing colon, if any.
					
					add_label(tok[j], curPc);
				break;

				case ARG_ENTER: // Inside instruction
					if(curPass == -1)
						break;	
				
					// Is this token a comment?
					// If so, we do not have arguments anymore.

				
					if(*s == ';')
					{
						state = COMMENT;
						break;
					}

					strcat(argbuf, s);
					
					int was_sp = 0;
					int in_string = 0;
					int stringt = 0;
					char *argPlace = &linebuf2[ tok[j] - tok[0] ];

					a = linebuf2;

					l = 0;
					while(l < j && (argPlace = strtok(a, " \t")))
					{
						a = NULL;
						l++;
					}
					
					while(*argPlace)argPlace++;
					while(!(*argPlace))argPlace++;
					
					char arg[64];
					char *argp = arg;
					int is_ok = 0;
					int esc = 0;
					
					rawArgc = 0;
					
					// Emulate a bug in Hitmen's assembler
					if(strcasecmp(curIns, "li") == 0)
					{
argplace_li_remove_spaces_begin:
						for(l = 0; argPlace[l]; l++)
						{
							if(argPlace[l] == ' ')
							{
								l++;
								for(; argPlace[l]; l++)
									argPlace[l-1] = argPlace[l];
								
								argPlace[l-1] = '\0';
								
								goto argplace_li_remove_spaces_begin;
							}
						}
					}
					
					for(l = 0; argPlace[l] && rawArgc < 64; l++)
					{
						char c = argPlace[l];
						
						if(in_string)
						{
							*(argp++) = c;
							
							if(!esc)
							{
								if(stringt == 0 && c == '"')
									in_string = 0;
								else if(stringt == 1 && c == '\'')
									in_string = 0;
								else if(c == '\\')
									esc = 1;
							}
							else
								esc = 0;
						}
						else
						{
							if(isalnum((unsigned int)c) || c == '_' || c == '$' || c == '.' || c == '*')
							{
								is_ok = 0;
								
								if(was_sp && (isalnum((unsigned int)*(argp-1)) || (*(argp-1) == '_') ||
										(*(argp-1) == '$') || (*(argp-1) == '"')
											|| (*(argp-1) == '\'') || (*(argp-1) == '.') || (*(argp-1) == '*')))
									goto noMoreArgs;
								
								*(argp++) = c;
								was_sp = 0;
							}
							else if(c == '"')
							{
								if(was_sp && (isalnum((unsigned int)*(argp-1)) || (*(argp-1) == '_') ||
										(*(argp-1) == '$') || (*(argp-1) == '"')
											|| (*(argp-1) == '\'') || (*(argp-1) == '.') || (*(argp-1) == '*')))
									goto noMoreArgs;
								
								*(argp++) = c;
								was_sp = 0;
								in_string = 1;
								stringt = 0;
								esc = 0;
							}
							else if(c == '\'')
							{
								if(was_sp && (isalnum((unsigned int)*(argp-1)) || (*(argp-1) == '_') ||
										(*(argp-1) == '$') || (*(argp-1) == '"')
											|| (*(argp-1) == '\'') || (*(argp-1) == '.') || (*(argp-1) == '*')))
									goto noMoreArgs;
								
								*(argp++) = c;
								was_sp = 0;
								in_string = 1;
								stringt = 1;
								esc = 0;
							}
							else if(c == ' ' || c == '\t')
							{
								is_ok = 0;
								was_sp = 1;
							}
							else if(c == '+' || c == '-' || c == '>' || c == '<'
								|| c == '(' || c == ')' || c == '&' || c == '|' || c == '!')
							{
								is_ok = 0;
								*(argp++) = c;
							}
							else if(c == ',')
							{
								*argp = '\0';
								insArgt[rawArgc] = 0;
								strcpy(rawArgv[rawArgc++], arg);
								argp = arg;
								was_sp = 0;
								is_ok = 1;
							}
							else if(c == ';' || c == '/')
							{
// '/' added in order to emulate a very buggy behavior of SPASM that is needed
// in order to assemble the imbNES 1.3.2 sources without modifications.
// yes, imbNES has some UNDENOTATED comments!
//
// pearls such as  
//								
// lw      v0,$1074(v1)    // this line would be at $DFAC where the jump goes from the patch								
//
// and...
//
// lw	v0,$DFFC(v0)	load the address to jump back to that was set in _patch_card
//
// It makes no sense, but hey it works in the original SPASM!								

								
								goto noMoreArgs;
							}
							else
							{
								instruction_error("Invalid character!\n");
								break;
							}
						}
					}
					
noMoreArgs:			
					if(!is_ok)
					{
						*argp = '\0';
						
						char *fb = strchr(arg, '(');
						char *sb = strrchr(arg, ')');

						
						int pa=(*arg != '"' && *arg != '\'' && fb && sb && fb<sb);

						if(pa)
						{
							*fb = '\0';
							*sb = '\0';
							insArgt[rawArgc+1] = 1;
							strcpy(rawArgv[rawArgc+1], fb+1);
						}
						
						insArgt[rawArgc] = 0;
						strcpy(rawArgv[rawArgc++], arg);
						
						if(pa)rawArgc++;
					}
			
					insArgc = 0;
					
					find_label_reset();

					for(l = 0; l < rawArgc; l++, insArgc++)
					{						
						if(!inMacro)
						{
						//	printf("rawArgv[%d] = %s\n", l, rawArgv[l]);
							insArgv[l] = spasm_eval(rawArgv[l]);	
						}
					}
					
					if(curPass == 1 && !find_label_ok())
						instruction_error("Can't resolve expression");
					
					goto theNextLine;
				break;
				
				case COMMENT: // Inside comment
					
				break;
			}
			
			a = NULL;
			j++;
		}

theNextLine:
		if(INSFUNC)
		{
			if(!inMacro)
			{
			/*	if(INSFUNC == THIS_IS_A_MACRO)
				{
					//printf("insArgc = %d, curMacro->argc = %d\n",
					//	insArgc, curMacro->argc);
					
					if(insArgc != curMacro->argc)
						instruction_error("Macro \"%s\" takes %d arguments, not %d",
							curMacro->name, curMacro->argc, insArgc);
					
					char **argv = malloc(sizeof(char*) * 16);
					
					for(m = 0; m < 16; m++)
						argv[m] = rawArgv[m];
					
					growingText *expandedMacro = expand_macro(curMacro, argv);
					
					free(argv);

					growingText *newText = newGText();
					addTextToGTextN(newText, curText, oldI);
					addTextToGText(newText, expandedMacro->text);
					addTextToGText(newText, curText+i);
					
					freeGText(expandedMacro);

					free(curText);
					curText = strdup(newText->text);
					freeGText(newText);
					
					goto theBeginning;
				}
				else
				{*/
					if(curPass>=0)INSFUNC();
				//}
					
				if(strcasecmp(curIns, "include") == 0 && curPass == 0)
				{
					goto theBeginning;
				}
			
				first_instruction = 0;
			}
		}
	}
	
	/*if(curPass == 0)
	{
		fprintf(logFile, "NUJ");
		fclose(logFile);
	}*/
	
	return curText;
}
