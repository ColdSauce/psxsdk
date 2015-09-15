#include "spasm.h"

/**
 * Expressions in SPASM are implemented in a totally broken manner.
 *
 * The result of an expression is the value of its initial argument
 * after executing the operation of the last operand with the last argument.
 *
 * For example dw $cafeba00+2+4 is not equal to dw $cafeba06
 * but to dw $cafeba04.
 * Likewise, dw $2+$cafeba00+4 is not equal to dw $cafeba06
 * but to dw $6 !
 */

unsigned int spasm_eval(char *expr)
{	
	char *cset = "+-><&|*!";
	char *csetp;
	char *cp;
	int ok;
	int t = T_INTEGER;
	char sbuf[128];
	
	if(strcmp(expr, "*") == 0) // Return current return address
		return curPc;
	
	if(strcasecmp(curIns, "incbin") == 0)
		return 0; // If current instruction is incbin, do not evaluate.
		
	if(*expr == '"' || *expr == '\'')
		return 0; // I won't even try to evaluate strings!
	
	int ispan = strcspn(expr, cset);
	
	csetp = cset;

	ok = 0; // ok will be 0 if we found no operator, 1 if we found it
	
	char op = '?';
	
	cp = NULL;
	
	while(*csetp)
	{
		char *tcp = strrchr(expr, *csetp);
		
		if(tcp) // Found operator
		{
			if(tcp > cp)
				cp = tcp;
			
			if(*cp == '>')
			{				
				if(*(cp-1) != '>')
					instruction_error("Bad operator");
			}
			
			if(*cp == '<')
			{
				if(*(cp-1) != '<')
					instruction_error("Bad operator");
			}
				
			cp++;
			ok = 1;
			op = *csetp;
			break;
		}
		
		csetp++;
	}
	
	unsigned int one = 0;
	unsigned int two = 0;
	
	memcpy(sbuf, expr, ispan);
	sbuf[ispan] = '\0';
	
	if(strlen(sbuf) == 0)
		strcpy(sbuf, "0");
		//instruction_error("Bad expression");
	
	//printf("sbuf = %s\n", sbuf);
	one = asm_atoi(sbuf);
	
	t = atoiT[insArgc];
	
	if(ok)
	{		
		strcpy(sbuf, cp);
		
		if(strlen(sbuf) == 0)
			instruction_error("Bad expression");
		
		two = asm_atoi(sbuf);
		
		if(t == T_INTEGER)
			t = atoiT[insArgc];
	}
	
	atoiT[insArgc] = t;
	
	switch(op)
	{
		case '+':
			return one + two;
		break;
		case '-':
			return one -  two;
		break;
		case '<':
			return one << two;
		break;
		case '>':
			return one >> two;
		break;
		case '&':
			return one & two;
		break;
		case '|':
		case '!':
			return one | two;
		break;
		case '*':
			return one * two;
		break;
	}
	
	return one;
}
