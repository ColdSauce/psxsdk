#ifndef _SPASM_OPCODE_H
#define _SPASM_OPCODE_H

// Helper functions

void OUTSEEK(unsigned int position);
void OUTBYTE(unsigned char b);
void OUTHALF(unsigned short h);
void OUTWORD(unsigned int w);
void OUTINS(unsigned int instruction);
void OUTSTRING(char *string);
unsigned int OUTSIZE(void);
void *get_instruction(char *name);

// Instructions

void INS_ADD(void);
void INS_ADDI(void);
void INS_ADDIU(void);
void INS_ADDU(void);
void INS_AND(void);
void INS_ANDI(void);
void INS_BEQ(void);
void INS_BGEZ(void);
void INS_BGEZAL(void);
void INS_BGTZ(void);
void INS_BLEZ(void);
void INS_BLTZ(void);
void INS_BLTZAL(void);
void INS_BNE(void);
void INS_BREAK(void);
void INS_CFC(void);
void INS_COP(void);
void INS_CTC(void);
void INS_DIV(void);
void INS_DIVU(void);
void INS_J(void);
void INS_JAL(void);
void INS_JALR(void);
void INS_JR(void);
void INS_LB(void);
void INS_LBU(void);
void INS_LH(void);
void INS_LHU(void);
void INS_LUI(void);
void INS_LW(void);
void INS_LWC(void);
void INS_LWL(void);
void INS_LWR(void);
void INS_MFC(void);
void INS_MFHI(void);
void INS_MFLO(void);
void INS_MTC(void);
void INS_MTHI(void);
void INS_MTLO(void);
void INS_MULT(void);
void INS_MULTU(void);
void INS_NOR(void);
void INS_OR(void);
void INS_ORI(void);
void INS_SB(void);
void INS_SH(void);
void INS_SLL(void);
void INS_SLLV(void);
void INS_SLT(void);
void INS_SLTI(void);
void INS_SLTIU(void);
void INS_SLTU(void);
void INS_SRA(void);
void INS_SRAV(void);
void INS_SRL(void);
void INS_SRLV(void);
void INS_SUB(void);
void INS_SUBU(void);
void INS_SW(void);
void INS_SWC(void);
void INS_SWL(void);
void INS_SWR(void);
void INS_SYSCALL(void);
void INS_XOR(void);
void INS_XORI(void);

// Pseudo instructions

void INS_B(void);
void INS_LI(void);
void INS_LA(void);
void INS_NOP(void);
void INS_MOVE(void);
void INS_SUBI(void);
void INS_SUBIU(void);
void INS_BEQZ(void);
void INS_BNEZ(void);
void INS_BAL(void);
void INS_ORG(void);
void INS_INCBIN(void);
void INS_INCLUDE(void);
void INS_DCB(void);
void INS_DB(void);
void INS_DH(void);
void INS_DW(void);
void INS_ALIGN(void);

// Fake instructions

void INS_BLANK(void);

#endif
