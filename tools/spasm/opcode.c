#include "spasm.h"

struct
{
	char *name;
	void (*func)();
}instruction_table[] =
{
	{"add"		,INS_ADD},
	{"addi"		,INS_ADDI},
	{"addiu"		,INS_ADDIU},
	{"addu"		,INS_ADDU},
	{"and"		,INS_AND},
	{"andi"		,INS_ANDI},
	{"beq"		,INS_BEQ},
	{"bgez"		,INS_BGEZ},
	{"bgezal"		,INS_BGEZAL},
	{"bgtz"		,INS_BGTZ},
	{"blez"		,INS_BLEZ},
	{"bltz"		,INS_BLTZ},
	{"bltzal"		,INS_BLTZAL},
	{"bne"		,INS_BNE},
	{"break"		,INS_BREAK},
	{"cfc0"		,INS_CFC },
	{"cfc1"		,INS_CFC },
	{"cfc2"		,INS_CFC },
	{"cfc3"		,INS_CFC },
	{"cop0"		,INS_COP },
	{"cop1"		,INS_COP },
	{"cop2"		,INS_COP },
	{"cop3"		,INS_COP },
	{"ctc0"		,INS_CTC },
	{"ctc1"		,INS_CTC },
	{"ctc2"		,INS_CTC },
	{"ctc3"		,INS_CTC },
	{"div"		,INS_DIV},
	{"divu"		,INS_DIVU},
	{"j"			,INS_J},
	{"jal"		,INS_JAL},
	{"jalr"		,INS_JALR},
	{"jr"			,INS_JR},
	{"lb"		,INS_LB},
	{"lbu"		,INS_LBU},
	{"lh"		,INS_LH},
	{"lhu"		,INS_LHU},
	{"lui"		,INS_LUI},
	{"lw"		,INS_LW},
	{"lwc0"		,INS_LWC },
	{"lwc1"		,INS_LWC },
	{"lwc2"		,INS_LWC },
	{"lwc3"		,INS_LWC },
	{"lwl"		,INS_LWL},
	{"lwr"		,INS_LWR},
	{"mfc0"		,INS_MFC},
	{"mfc1"		,INS_MFC},
	{"mfc2"		,INS_MFC},
	{"mfc3"		,INS_MFC},
	{"mfhi"		,INS_MFHI},
	{"mflo"		,INS_MFLO},
	{"mtc0"		,INS_MTC },
	{"mtc1"		,INS_MTC },
	{"mtc2"		,INS_MTC },
	{"mtc3"		,INS_MTC },
	{"mthi"		,INS_MTHI},
	{"mtlo"		,INS_MTLO},
	{"mult"		,INS_MULT},
	{"multu"		,INS_MULTU},
	{"nor"		,INS_NOR},
	{"or"		,INS_OR},
	{"ori"		,INS_ORI},
	{"sb"		,INS_SB},
	{"sh"		,INS_SH},
	{"sll"		,INS_SLL},
	{"sllv"		,INS_SLLV},
	{"slt"		,INS_SLT},
	{"slti"		,INS_SLTI},
	{"sltiu"		,INS_SLTIU},
	{"sltu"		,INS_SLTU},
	{"sra"		,INS_SRA},
	{"srav"		,INS_SRAV},
	{"srl"		,INS_SRL},
	{"srlv"		,INS_SRLV},
	{"sub"		,INS_SUB},
	{"subu"		,INS_SUBU},
	{"sw"		,INS_SW},
	{"swc0"		,INS_SWC},
	{"swc1"		,INS_SWC},
	{"swc2"		,INS_SWC},
	{"swc3"		,INS_SWC},
	{"swl"		,INS_SWL},
	{"swr"		,INS_SWR},
	{"syscall"		,INS_SYSCALL},
	{"xor"		,INS_XOR},
	{"xori"		,INS_XORI},
	
	{"b"			,INS_B},
	{"la"		,INS_LA},
	{"li"			,INS_LI},
	{"nop"		,INS_NOP},
	{"move"		,INS_MOVE},
	{"subi"		,INS_SUBI},
	{"subiu"		,INS_SUBIU},
	{"beqz"		,INS_BEQZ},
	{"bnez"		,INS_BNEZ},
	{"bal"		,INS_BAL},
	{"org"		,INS_ORG},
	{"include"	,INS_INCLUDE},
	{"incbin"		,INS_INCBIN},
	{"dcb"		,INS_DCB},
	{"db"		,INS_DB},
	{"dh"		,INS_DH},
	{"dw"		,INS_DW},
	{"align"		,INS_ALIGN},
	
	{NULL},
};

#define I_TYPE(op, rs, rt, imm) \
	( (((op) & 63) << 26) | (((rs) & 31) << 21) | (((rt) & 31) << 16) | \
		((imm) & 0xFFFF) )
		
#define J_TYPE(op, target) \
	( (((op) & 63) << 26) | ((target) & 0x3FFFFFF) )

#define R_TYPE(op, rs, rt, rd, shamt, funct) \
	( (((op) & 63) << 26) | (((rs) & 31) << 21) | (((rt) & 31) << 16) | \
		(((rd) & 31) << 11) | (((shamt) & 31) << 6) | \
			((funct) & 63))

// copn = coprocessor number
// SET_DIS_CHECK()

#define SET_DIS_CHECK()	/* Argument type check. Placeholder, contains nothing for now. */

int set_delay_slot = 0;

void OUTSEEK(unsigned int position)
{
	fseek(asmOut, position, SEEK_SET);
}

void OUTBYTE(unsigned char b)
{
	if(curPass>0)
		fputc(b, asmOut);
	
	curPc++;
}

void OUTHALF(unsigned short h)
{
	if(curPass>0)
	{
		fputc(h&0xff, asmOut);
		fputc(h>>8, asmOut);
	}
	
	curPc += 2;
}

void OUTWORD(unsigned int w)
{
	if(curPass>0)
	{
		fputc(w&0xff, asmOut);
		fputc((w>>8)&0xff, asmOut);
		fputc((w>>16)&0xff, asmOut);
		fputc(w>>24, asmOut);
	}
	
	curPc += 4;
}

void OUTINS(unsigned int instruction)
{	
	OUTWORD(instruction);
	
	if(set_delay_slot)
	{
		OUTWORD(0);

		set_delay_slot = 0;
	}
}


void OUTSTRING(char *string)
{
	int stringt;
	int esc=0;
	
	if(*string == '"')
		stringt = 0;
	else if(*string == '\'')
		stringt = 1;
	else
		instruction_error("OUTSTRING <INTERNAL ERROR>. Not a string!");
	
	string++;
	
	while(*string)
	{
		if(*string == '"')
		{
			if(stringt == 0 && !esc)
				break;
			
			OUTBYTE('"');
			esc = 0;
		}
		else if(*string == '\'')
		{
			if(stringt == 1 && !esc)
				break;
			
			OUTBYTE('\'');
			esc = 0;
		}
		else if(*string == 'n')
		{
			if(esc)
				OUTBYTE('\n');
			else
				OUTBYTE('n');
			
			esc = 0;
		}
		else if(*string == 't')
		{
			if(esc)
				OUTBYTE('\t');
			else
				OUTBYTE('t');
			
			esc = 0;
		}
		else if(*string == 'r')
		{
			if(esc)
				OUTBYTE('\r');
			else
				OUTBYTE('r');
			
			esc = 0;
		}
		else if(*string == '\\')
		{
			if(esc)
			{
				OUTBYTE('\\');
				esc = 0;
			}
			else
				esc = 1;
		}
		else
		{
			if(esc)
				instruction_error("Invalid escape sequence \\%c in string", *string);
			
			OUTBYTE(*string);
		}
		
		string++;
	}
}

unsigned int OUTSIZE(void)
{
	int r;
	int pos = ftell(asmOut);
	fseek(asmOut, 0, SEEK_END);
	r = ftell(asmOut);
	fseek(asmOut, pos, SEEK_SET);
	return r;
}
	
unsigned short compute_branch(unsigned int imm)
{	
	unsigned int off = imm - (curPc + 4);
	//off >>= 2;
	
	if(curPass <= 0)
		return 0;
	
	if(off >= 0x20000 && off < -0x20000)
		instruction_error("Branch out of range. %04x", off);
		
	return (off>>2) & 0xFFFF;
}

unsigned int compute_jump(unsigned int imm)
{	
	if(curPass <= 0)
		return 0;

	return (imm >> 2);
}

struct
{
	int size;
	int pos;
	unsigned int *el;
}cannotPredict = {0, 0, NULL};

unsigned int compute_real_offset(unsigned int imm, unsigned int base,
	unsigned int rt)
{
	int i, unpredictable=0;
	unsigned short hipart;
	unsigned short lopart;
	
	if(!find_label_ok())
	{
		if(cannotPredict.size == cannotPredict.pos)
		{
			cannotPredict.size += 128;
			cannotPredict.el = realloc(cannotPredict.el, cannotPredict.size * sizeof(int));
		}
		
		cannotPredict.el[cannotPredict.pos++] = curPc;
	}
	
	for(i = 0; i < cannotPredict.pos; i++)
	{
		if(curPc == cannotPredict.el[i])
		{
			unpredictable = 1;
			break;
		}
	}
	
	if(!unpredictable && ((imm <= 0xFFFF) || (imm >= 0xFFFF8000 && imm <= 0xFFFFFFFF)))
		return I_TYPE(0, base, rt, imm);

//compute_real_offset_output_wide:
	// li at, offset
	// add at, at, base
	// lw rt, 0(at)
	hipart = (imm >> 16);
	lopart = imm & 0xFFFF;
	int t=(*curIns == 'l') ? rt : 1;
	
	// lw at, $CAFEBABE(zero) -> lui at, $CAFE, ori at, at, $BABE, lw at, 0(at)
	// lw at, $CAFEBABE(v0) -> lui at, $CAFE, ori at, at, $BABE, lw at, 0(v0)
	// sw at, $CAFEBABE -> 
	
	if(base)
	{
		/*OUTINS( I_TYPE(15, 0, t, hipart)); // lui $t, (offset > 16)
		
		if(lopart || unpredictable)
			OUTINS( I_TYPE (13, t, t, lopart)); // ori $t, $t, offset & 0xffff
		
		OUTINS( R_TYPE (0, t, base, t, 0, 33) ); // add $t, $t, base
		return I_TYPE(0, t, rt, 0);*/
		
// SPASM is seriously broken regarding this..
		return I_TYPE(0, base, rt, imm);
	}
	
	if(lopart >= 0x8000)
		hipart++;
		
	OUTINS( I_TYPE(15, 0, t, hipart)); // lui $t, (offset > 16)
		
	return I_TYPE(0, t, rt, lopart); // XX rt, lopart(rt)
}

void INS_ADD(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	// ADD rd, rs, rt -> rd = rs + rt
	
	if(insArgc == 2)
	{
		if(atoiT[1] == T_INTEGER)
			return INS_ADDI();
			
		rd = insArgv[0];
		rs = insArgv[0];
		rt = insArgv[1];
	}
	else
	{
		if(atoiT[2] == T_INTEGER)
			return INS_ADDI();
		
		rd = insArgv[0];
		rs = insArgv[1];
		rt = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 32) );
}

void INS_ADDI(void)
{
	unsigned int rt, rs, imm;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	// ADDI rt, rs, imm -> rt = rs + imm;
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		rs = insArgv[0];
		imm = insArgv[1];
	}
	else
	{
		rt = insArgv[0];
		rs = insArgv[1];
		imm = insArgv[2];
	}
	
	if(imm > 0x7FFF && imm < 0xFFFF0000)
		instruction_warning("Immediate is possibly out of range.");
	
	OUTINS( I_TYPE (8, rs, rt, imm) );
}

void INS_ADDIU(void)
{
	unsigned int rt, rs, imm;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	// ADDIU rt, rs, imm -> rt = rs + imm;
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		rs = insArgv[0];
		imm = insArgv[1];
	}
	else
	{
		rt = insArgv[0];
		rs = insArgv[1];
		imm = insArgv[2];
	}
	
	if(imm > 0x7FFF && imm < 0xFFFF0000)
		instruction_warning("Immediate is possibly out of range.");
	
	OUTINS( I_TYPE (9, rs, rt, imm) );
}

void INS_ADDU(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	// ADDU rd, rs, rt -> rd = rs + rt
	
	if(insArgc == 2)
	{
		if(atoiT[1] == T_INTEGER)
			return INS_ADDIU();
		
		rd = insArgv[0];
		rs = insArgv[0];
		rt = insArgv[1];
	}
	else
	{
		if(atoiT[2] == T_INTEGER)
			return INS_ADDIU();
		
		rd = insArgv[0];
		rs = insArgv[1];
		rt = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 33) );
}

void INS_AND(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	// AND rd, rs, rt -> rd = rs + rt
	
	if(insArgc == 2)
	{
		if(atoiT[1] == T_INTEGER)
			return INS_ANDI();
		
		rd = insArgv[0];
		rs = insArgv[0];
		rt = insArgv[1];
	}
	else
	{
		if(atoiT[2] == T_INTEGER)
			return INS_ANDI();
		
		rd = insArgv[0];
		rs = insArgv[1];
		rt = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 36) );
}

void INS_ANDI(void)
{
	unsigned int rt, rs, imm;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	// ANDI rt, rs, imm -> rt = rs + imm;
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		rs = insArgv[0];
		imm = insArgv[1];
	}
	else
	{
		rt = insArgv[0];
		rs = insArgv[1];
		imm = insArgv[2];
	}
	
	if(imm > 0xFFFF)
		instruction_warning("Immediate is possibly out of range");
	
	OUTINS( I_TYPE (12, rs, rt, imm) );
}

void INS_BEQ(void)
{
	unsigned int rt, rs, imm;
	
	if(insArgc != 3)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	rt = insArgv[1];
	imm = insArgv[2];
	
	OUTINS( I_TYPE(4, rs, rt, compute_branch(imm)) );
}

void INS_BGEZ(void)
{
	unsigned int rs, imm;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	imm = insArgv[1];
	
	OUTINS( I_TYPE(1, rs, 1, compute_branch(imm)) );
}

void INS_BGEZAL(void)
{
	unsigned int rs, imm;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	imm = insArgv[1];
	
	OUTINS( I_TYPE(1, rs, 17, compute_branch(imm)) );
}

void INS_BGTZ(void)
{
	unsigned int rs, imm;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	imm = insArgv[1];
	
	OUTINS( I_TYPE(7, rs, 0, compute_branch(imm)) );
}

void INS_BLEZ(void)
{
	unsigned int rs, imm;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	imm = insArgv[1];
	
	OUTINS( I_TYPE(6, rs, 0, compute_branch(imm)) );
}

void INS_BLTZ(void)
{
	unsigned int rs, imm;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	imm = insArgv[1];
	
	OUTINS( I_TYPE(1, rs, 0, compute_branch(imm)) );
}

void INS_BLTZAL(void)
{
	unsigned int rs, imm;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	imm = insArgv[1];
	
	OUTINS( I_TYPE(1, rs, 16, compute_branch(imm)) );
}

void INS_BNE(void)
{
	unsigned int rt, rs, imm;
	
	if(insArgc != 3)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	rt = insArgv[1];
	imm = insArgv[2];
	
	OUTINS( I_TYPE(5, rs, rt, compute_branch(imm)) );
}

void INS_BREAK(void)
{
	unsigned int imm = 0;
	
	if(insArgc > 1)
		instruction_error("Too many arguments");
	
	if(insArgc == 1)
		imm = insArgv[0];
	
	imm &= 0xFFFFF;
	
	OUTINS( (imm << 6) | 13 );
}

void INS_CFC(void)
{
	unsigned int rt, rd;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rt = insArgv[0];
	rd = insArgv[1];
	
	OUTINS ( R_TYPE(16 | copn, 2, rt, rd, 0, 0) );
}

void INS_COP(void)
{
	unsigned int cofun;
	
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	cofun = insArgv[0];
	
	OUTINS( ( (16 | copn) << 26) | (1<<25) | (cofun & 0x1FFFFFF));
}

void INS_CTC(void)
{
	unsigned int rt, rd;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rt = insArgv[0];
	rd = insArgv[1];
	
	OUTINS ( R_TYPE(16 | copn, 6, rt, rd, 0, 0) );
}

void INS_DIV(void)
{
	unsigned int rs, rt;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	rt = insArgv[1];
	
	OUTINS ( R_TYPE(0, rs, rt, 0, 0, 26));
}

void INS_DIVU(void)
{
	unsigned int rs, rt;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	rt = insArgv[1];
	
	OUTINS ( R_TYPE(0, rs, rt, 0, 0, 27));
}

void INS_J(void)
{	
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	OUTINS ( J_TYPE(2, compute_jump(insArgv[0])));
}

void INS_JAL(void)
{	
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	OUTINS ( J_TYPE(3, compute_jump(insArgv[0])));
}

void INS_JALR(void)
{
	unsigned int rd, rs;
	
	if(insArgc < 1)
		instruction_error("Not enough arguments");
	if(insArgc > 2)
		instruction_error("Too many arguments");
	
	if(insArgc == 1)
	{
		rd = 31; // register ra
		rs = insArgv[0];
	}
	else
	{
		rd = insArgv[0];
		rs = insArgv[1];
	}
	
	OUTINS ( R_TYPE(0, rs, 0, rd, 0, 9));
}

void INS_JR(void)
{
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	OUTINS ( R_TYPE(0, insArgv[0], 0, 0, 0, 8));
}

void INS_LB(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(32, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}		
	
void INS_LBU(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(36, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_LH(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(33, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_LHU(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(37, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_LUI(void)
{	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	OUTINS(I_TYPE(15, 0, insArgv[0], insArgv[1]));
}

void INS_LW(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(35, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_LWC(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(48 | copn, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_LWL(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(34, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_LWR(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(38, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_MFC(void)
{
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	OUTINS(R_TYPE (16 | copn, 0, insArgv[0], insArgv[1], 0, 0));
}

void INS_MFHI(void)
{
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	OUTINS(R_TYPE(0, 0, 0, insArgv[0], 0, 16));
}

void INS_MFLO(void)
{
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	OUTINS(R_TYPE(0, 0, 0, insArgv[0], 0, 18));
}

void INS_MTC(void)
{
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	OUTINS(R_TYPE (16 | copn, 4, insArgv[0], insArgv[1], 0, 0));
}

void INS_MTHI(void)
{
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	OUTINS(R_TYPE(0, insArgv[0], 0, 0, 0, 17));
}

void INS_MTLO(void)
{
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	OUTINS(R_TYPE(0, insArgv[0], 0, 0, 0, 19));
}

void INS_MULT(void)
{
	unsigned int rs, rt;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	rt = insArgv[1];
	
	OUTINS(R_TYPE(0, rs, rt, 0, 0, 24));
}

void INS_MULTU(void)
{
	unsigned int rs, rt;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rs = insArgv[0];
	rt = insArgv[1];
	
	OUTINS(R_TYPE(0, rs, rt, 0, 0, 25));
}

void INS_NOR(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");

	if(insArgc == 2)
	{
		rd = insArgv[0];
		rs = insArgv[0];
		rt = insArgv[1];
	}
	else
	{
		rd = insArgv[0];
		rs = insArgv[1];
		rt = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 39) );
}

void INS_OR(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");

	if(insArgc == 2)
	{
		if(atoiT[1] == T_INTEGER)
			return INS_ORI();
		
		rd = insArgv[0];
		rs = insArgv[0];
		rt = insArgv[1];
	}
	else
	{
		if(atoiT[2] == T_INTEGER)
			return INS_ORI();
		
		rd = insArgv[0];
		rs = insArgv[1];
		rt = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 37) );
}

void INS_ORI(void)
{
	unsigned int rt, rs, imm;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	// ANDI rt, rs, imm -> rt = rs + imm;
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		rs = insArgv[0];
		imm = insArgv[1];
	}
	else
	{
		rt = insArgv[0];
		rs = insArgv[1];
		imm = insArgv[2];
	}
	
	if(imm > 0xFFFF)
		instruction_warning("Immediate is possibly out of range");
	
	OUTINS( I_TYPE (13, rs, rt, imm) );
}

void INS_SB(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(40, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_SH(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(41, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_SLL(void)
{
	unsigned int rd, rt, sa;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rd = insArgv[0];
		rt = insArgv[0];
		sa = insArgv[1];
	}
	else
	{
		rd =  insArgv[0];
		rt = insArgv[1];
		sa = insArgv[2];
	}
	
	OUTINS(R_TYPE(0, 0, rt, rd, sa, 0));
}

void INS_SLLV(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");

	if(insArgc == 2)
	{
		rd = insArgv[0];
		rt = insArgv[0];
		rs = insArgv[1];
	}
	else
	{
		rd = insArgv[0];
		rt = insArgv[1];
		rs = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 4) );
}

void INS_SLT(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc != 3)
		instruction_error("Wrong number of arguments");

	if(atoiT[2] == T_INTEGER)
		return INS_SLTI();
	
	rd = insArgv[0];
	rs = insArgv[1];
	rt = insArgv[2];
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 42) );
}

void INS_SLTI(void)
{
	unsigned int imm, rs, rt;
	
	if(insArgc != 3)
		instruction_error("Wrong number of arguments");

	rt = insArgv[0];
	rs = insArgv[1];
	imm = insArgv[2];
	
	if(imm > 0x7FFF && imm < 0xFFFF8000)
		instruction_error("Immediate out of range.");
	
	OUTINS( I_TYPE (10, rs, rt, imm) );
}

void INS_SLTIU(void)
{
	unsigned int imm, rs, rt;
	
	if(insArgc != 3)
		instruction_error("Wrong number of arguments");

	rt = insArgv[0];
	rs = insArgv[1];
	imm = insArgv[2];
	
	if(imm > 0x7FFF && imm < 0xFFFF8000)
		instruction_error("Immediate out of range.");
	
	OUTINS( I_TYPE (11, rs, rt, imm) );
}

void INS_SLTU(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc != 3)
		instruction_error("Wrong number of arguments");

	if(atoiT[2] == T_INTEGER)
		return INS_SLTIU();
	
	rd = insArgv[0];
	rs = insArgv[1];
	rt = insArgv[2];
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 43) );
}

void INS_SRA(void)
{
	unsigned int rd, rt, sa;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rd = insArgv[0];
		rt = insArgv[0];
		sa = insArgv[1];
	}
	else
	{
		rd =  insArgv[0];
		rt = insArgv[1];
		sa = insArgv[2];
	}
	
	OUTINS(R_TYPE(0, 0, rt, rd, sa, 3));
}

void INS_SRAV(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");

	if(insArgc == 2)
	{
		rd = insArgv[0];
		rt = insArgv[0];
		rs = insArgv[1];
	}
	else
	{
		rd = insArgv[0];
		rt = insArgv[1];
		rs = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 7) );
}

void INS_SRL(void)
{
	unsigned int rd, rt, sa;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rd = insArgv[0];
		rt = insArgv[0];
		sa = insArgv[1];
	}
	else
	{
		rd =  insArgv[0];
		rt = insArgv[1];
		sa = insArgv[2];
	}
	
	OUTINS(R_TYPE(0, 0, rt, rd, sa, 2));
}

void INS_SRLV(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");

	if(insArgc == 2)
	{
		rd = insArgv[0];
		rt = insArgv[0];
		rs = insArgv[1];
	}
	else
	{
		rd = insArgv[0];
		rt = insArgv[1];
		rs = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 6) );
}

void INS_SUB(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	
	if(insArgc == 2)
	{
		if(atoiT[1] == T_INTEGER)
			return INS_SUBI();
		
		rd = insArgv[0];
		rs = insArgv[0];
		rt = insArgv[1];
	}
	else
	{
		if(atoiT[2] == T_INTEGER)
			return INS_SUBI();
		
		rd = insArgv[0];
		rs = insArgv[1];
		rt = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 34) );
}

void INS_SUBU(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	// ADD rd, rs, rt -> rd = rs + rt
	
	if(insArgc == 2)
	{
		if(atoiT[1] == T_INTEGER)
			return INS_SUBIU();
		
		rd = insArgv[0];
		rs = insArgv[0];
		rt = insArgv[1];
	}
	else
	{
		if(atoiT[2] == T_INTEGER)
			return INS_SUBIU();
		
		rd = insArgv[0];
		rs = insArgv[1];
		rt = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 35) );
}

void INS_SW(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(43, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_SWC(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(56 | copn, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_SWL(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(42, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_SWR(void)
{
	unsigned int base, rt, offset;
	
	SET_DIS_CHECK();
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = 0;
	}
	else
	{
		rt = insArgv[0];
		offset = insArgv[1];
		base = insArgv[2];
	}
	
	OUTINS(R_TYPE(46, 0, 0, 0, 0, 0) | compute_real_offset(offset, base, rt));
}

void INS_SYSCALL(void)
{
	unsigned int imm = 0;
	
	if(insArgc > 1)
		instruction_error("Too many arguments");
	
	if(insArgc == 1)
		imm = insArgv[0];
	
	imm &= 0xFFFFF;
	
	OUTINS( (imm << 6) | 12 );
}

void INS_XOR(void)
{
	unsigned int rd, rs, rt;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");

	if(insArgc == 2)
	{
		if(atoiT[1] == T_INTEGER)
			return INS_XORI();
		
		rd = insArgv[0];
		rs = insArgv[0];
		rt = insArgv[1];
	}
	else
	{
		if(atoiT[2] == T_INTEGER)
			return INS_XORI();
		
		rd = insArgv[0];
		rs = insArgv[1];
		rt = insArgv[2];
	}
	
	OUTINS( R_TYPE (0, rs, rt, rd, 0, 38) );
}

void INS_XORI(void)
{
	unsigned int rt, rs, imm;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");

	if(insArgc == 2)
	{
		rt = insArgv[0];
		rs = insArgv[0];
		imm = insArgv[1];
	}
	else
	{
		rt = insArgv[0];
		rs = insArgv[1];
		imm = insArgv[2];
	}
	
	if(imm > 0xFFFF)
		instruction_warning("Immediate is possibly out of range");
	
	OUTINS( I_TYPE (14, rs, rt, imm) );
}

// ***** PSEUDO INSTRUCTIONS *****

void INS_B(void)
{
	unsigned int imm;
	
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	imm = insArgv[0];
	
	OUTINS( I_TYPE(4, 0, 0, compute_branch(imm)) ); // <- beq zero, zero, imm
}

/*void INS_LI(void)
{
	unsigned int rd, imm;
	unsigned short lopart, hipart;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rd = insArgv[0];
	imm = insArgv[1];
	
	hipart = imm >> 16;
	lopart = imm & 0xFFFF;

	if(atoiT[1] == T_INTEGER && imm >= 0 && imm <= 0xFFFF)
		OUTINS(I_TYPE(13, 0, rd, lopart)); // ori $rd, $zero, imm
	else
	{
		if(lopart >= 0x8000)
			hipart++;
		
		OUTINS( I_TYPE(15, 0, rd, hipart)); // lui $rd, (imm > 16)
		OUTINS( I_TYPE (9, rd, rd, lopart)); // addiu $rd, $rd, imm & 0xffff
	}
}*/

void INS_LI(void)
{
	int i;
	unsigned int rd, imm;
	unsigned short lopart, hipart;
	int unpredictable=0;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rd = insArgv[0];
	imm = insArgv[1];
	
	hipart = imm >> 16;
	lopart = imm & 0xFFFF;

	if(!find_label_ok())
	{
		if(cannotPredict.size == cannotPredict.pos)
		{
			cannotPredict.size += 128;
			cannotPredict.el = realloc(cannotPredict.el, cannotPredict.size * sizeof(int));
		}
		
		cannotPredict.el[cannotPredict.pos++] = curPc;
	}
	
	for(i = 0; i < cannotPredict.pos; i++)
	{
		if(curPc == cannotPredict.el[i])
		{
			unpredictable = 1;
			break;
		}
	}
	
	if(/*atoiT[1] == T_INTEGER &&*/ !unpredictable && imm >= 0 && imm <= 0xFFFF)
		OUTINS(I_TYPE(13, 0, rd, lopart)); // ori $rd, $zero, imm
	else if(!unpredictable && !lopart)
		OUTINS(I_TYPE(15, 0, rd, hipart));
	else
	{		
	//	if(lopart >= 0x8000)
	//		hipart++;
		
		OUTINS( I_TYPE(15, 0, rd, hipart)); // lui $rd, (imm > 16)

	//	OUTINS( I_TYPE(9, rd, rd, lopart)); // addiu $rd, $rd, imm & 0xffff
		OUTINS( I_TYPE (13, rd, rd, lopart)); // ori $rd, $rd, imm & 0xffff
	}
	
}

void INS_LA(void)
{
	int i;
	unsigned int rd, imm;
	unsigned short lopart, hipart;
	int unpredictable=0;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rd = insArgv[0];
	imm = insArgv[1];
	
	hipart = imm >> 16;
	lopart = imm & 0xFFFF;

	if(!find_label_ok())
	{
		if(cannotPredict.size == cannotPredict.pos)
		{
			cannotPredict.size += 128;
			cannotPredict.el = realloc(cannotPredict.el, cannotPredict.size * sizeof(int));
		}
		
		cannotPredict.el[cannotPredict.pos++] = curPc;
	}
	
	for(i = 0; i < cannotPredict.pos; i++)
	{
		if(curPc == cannotPredict.el[i])
		{
			unpredictable = 1;
			break;
		}
	}
	
	if(/*atoiT[1] == T_INTEGER &&*/ !unpredictable && imm >= 0 && imm <= 0xFFFF)
		OUTINS(I_TYPE(13, 0, rd, lopart)); // ori $rd, $zero, imm
	//else if(!unpredictable && !lopart)
	//	OUTINS(I_TYPE(15, 0, rd, hipart));
	else
	{		
		if(lopart >= 0x8000)
			hipart++;
		
		OUTINS( I_TYPE(15, 0, rd, hipart)); // lui $rd, (imm > 16)

		OUTINS( I_TYPE(9, rd, rd, lopart)); // addiu $rd, $rd, imm & 0xffff
//		OUTINS( I_TYPE (13, rd, rd, lopart)); // ori $rd, $rd, imm & 0xffff
	}
	
}

/*void INS_LA(void)
{
	INS_LI(); // The LI and LA pseudo-instructions are the same thing in SPASM
}*/

void INS_NOP(void)
{
	OUTINS(0);
}

void INS_MOVE(void)
{
	unsigned int rd, rs;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rd = insArgv[0];
	rs = insArgv[1];
	
	OUTINS( R_TYPE (0, rs, 0, rd, 0, 33) );  // addu $rd, $rs, $zero
}

void INS_SUBI(void)
{
// just like ADDI, but switches the sign of the immediate	
	
	unsigned int rt, rs, imm;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
	
	
	if(insArgc == 2)
	{
		rt = insArgv[0];
		rs = insArgv[0];
		imm = -insArgv[1];
	}
	else
	{
		rt = insArgv[0];
		rs = insArgv[1];
		imm = -insArgv[2];
	}
	
	if(imm > 0x7FFF && imm < 0xFFFF0000)
		instruction_warning("Immediate is possibly out of range");
	
	OUTINS( I_TYPE (8, rs, rt, imm) );
}

void INS_SUBIU(void)
{
	unsigned int rt, rs, imm;
	
	if(insArgc < 2)
		instruction_error("Not enough arguments");
	if(insArgc > 3)
		instruction_error("Too many arguments");
		
	if(insArgc == 2)
	{
		rt = insArgv[0];
		rs = insArgv[0];
		imm = -insArgv[1];
	}
	else
	{
		rt = insArgv[0];
		rs = insArgv[1];
		imm = -insArgv[2];
	}
	
	if(imm > 0x7FFF && imm < 0xFFFF0000)
		instruction_warning("Immediate is possibly out of range");
	
	OUTINS( I_TYPE (9, rs, rt, imm) );
}

void INS_BEQZ(void)
{
	unsigned int rt, imm;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rt = insArgv[0];
	imm = insArgv[1];
	
	OUTINS( I_TYPE(4, rt, 0, compute_branch(imm)) ); // <- beq zero, rt, imm
//		OUTINS( I_TYPE(4, rs, rt, compute_branch(imm)) );

}

void INS_BNEZ(void)
{
	unsigned int rt, imm;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	rt = insArgv[0];
	imm = insArgv[1];
	
	OUTINS( I_TYPE(5,rt, 0, compute_branch(imm)) ); // <- bne zero, rt, imm
}

void INS_BAL(void)
{
	unsigned int imm;
	
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	imm = insArgv[0];
	
	OUTINS( I_TYPE(1, 0, 17, compute_branch(imm)) ); //bgezal $zero, imm
}

void INS_ORG(void)
{	
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");

	//if(!first_instruction)
	//	instruction_error("ORG is not the first instruction");
	
	curPc = insArgv[0];
	startAddress = insArgv[0];
	org_found = 1;
}

void INS_INCBIN(void)
{
	FILE *f;
	char *path, *cp;
	int sz;
	
	
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	path = rawArgv[0];
	
	if(*path == '"')
	{
		path++;
		if((cp = strrchr(path, '"')))
			*cp = '\0';
	}
	else if(*path == '\'')
	{
		path++;
		if((cp = strrchr(path, '\'')))
			*cp = '\0';
	}
	
	//printf("DEBUG(INCBIN): including %s\n", path);
	
	f = spasm_fopen(path, "rb");
	
	if(!f)
		instruction_error("Could not open \"%s\"", path);
	
	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	if(curPass <= 0)
		curPc += sz;
	else
	{
		for(;sz;sz--)
			OUTBYTE(fgetc(f));
	}
	
	fclose(f);
}

void INS_DCB(void)
{
	unsigned int num, value;
	
	if(insArgc != 2)
		instruction_error("Wrong number of arguments");
	
	num = insArgv[0];
	value = insArgv[1];
	
	if(num & (1<<31))
	{
		instruction_warning("Negative number of values, ignoring instruction"); 
		
		return;
	}
	
	if(curPass <= 0)
		curPc += num;
	else
	{
		for(;num;num--)
			OUTBYTE(value);
	}
}

void INS_DB(void)
{
	int i;
	
	for(i = 0; i < insArgc; i++)
	{
		if(rawArgv[i][0] == '"' || rawArgv[i][0] == '\'')
			OUTSTRING(rawArgv[i]);
		else
			OUTBYTE(insArgv[i] & 0xFF);
	}
}

void INS_DH(void)
{
	int i;
	
	for(i = 0; i < insArgc; i++)
	{
		if(rawArgv[i][0] == '"' || rawArgv[i][0] == '\'')
			OUTSTRING(rawArgv[i]);
		else
			OUTHALF(insArgv[i] & 0xFFFF);
	}
}

void INS_DW(void)
{
	int i;
	
	for(i = 0; i < insArgc; i++)
	{
		if(rawArgv[i][0] == '"' || rawArgv[i][0] == '\'')
			OUTSTRING(rawArgv[i]);
		else
			OUTWORD(insArgv[i]);
	}
}

void INS_ALIGN(void)
{
	unsigned int unit;
	unsigned int delta;
	
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	unit = insArgv[0];
	
	if(unit <= 0)
		instruction_error("Alignment unit cannot be equal to zero or negative");
	
	if((curPc % unit))
	{
		delta = (unit - (curPc % unit)) % unit;
		
		if(curPass <= 0)
			curPc += delta;
		else
		{
			for(;delta;delta--)
				OUTBYTE(0);
		}
	}
}

void INS_INCLUDE(void)
{
	int i, l, o;
	int sz;
	int tsz;
	FILE *f;
	char *path;
	char *cp;
	char *newtext;
	
	if(insArgc != 1)
		instruction_error("Wrong number of arguments");
	
	if(curPass <= 0)
	{
		
		path = rawArgv[0];
	
		if(*path == '"')
		{
			path++;
			if((cp = strrchr(path, '"')))
				*cp = '\0';
		}
		else if(*path == '\'')
		{
			path++;
			if((cp = strrchr(path, '\'')))
				*cp = '\0';
		}
		
		f = spasm_fopen(path, "rb");
		
		if(!f)
			instruction_error("Could not open %s", path);
		
		//printf("DEBUG(INCLUDE): Including %s, line %d\n", path, line_number);
		
		fseek(f, 0, SEEK_END);
		sz = ftell(f);
		fseek(f, 0, SEEK_SET);
		
		tsz = strlen(curText);
		
		newtext = malloc(sz + tsz + 1);
		
		for(i = 0, l = 1; l < line_number; i++)
		{
			if(curText[i] == '\n')
				l++;
			
			newtext[i] = curText[i];
		}
		
		fread(&newtext[i], sizeof(char), sz, f);
		
		o = i+sz;
		
		if(newtext[i+sz-1] != '\n')
		{
			instruction_warning("No newline found at end of file %s", path);
			newtext[o++] = '\n';
		}
		
		
		for(; curText[i] != '\n'; i++);
		
		i++;
		newtext[o] = '\0';
		
		strcat(newtext, &curText[i]);
		
		free(curText);
		curText = newtext;
	}
}

void INS_BLANK(void)
{
	
}

void *get_instruction(char *name)
{
	int i;
	
	
	for(i = 0; instruction_table[i].name; i++)
	{
		//printf("name = ^%s$, iname = ^%s$\n", name, instruction_table[i].name);
		
		if(strcasecmp(name, instruction_table[i].name) == 0)
		{
		//	printf("di sranron\n");
			return instruction_table[i].func;
		}
	}
	
	return NULL;
}
