#include <psx.h>

unsigned int get_cop_register(unsigned char cop_num,
							unsigned char register_num)
{
// Workaround for MIPS' simplicistic instruction set...	
	
	unsigned int instr[] =
		{0x40020000, // cfc $v0, 0
		  0x03E00008, // jr $ra
		  0x00000000, // nop
                  0x00000000}; // nop

	int (*rawFunc)() = (void*)instr;

// Write coprocessor register number inside instruction		  
	instr[0] |= ( (cop_num & 3) << 26 ) | ( (register_num & 31) << 11 );

// Execute modified instruction
	return rawFunc();
}

unsigned int get_cop_ctrl_register(unsigned char cop_num,
							unsigned char register_num)
{
// Workaround for MIPS' simplicistic instruction set...	
	unsigned int instr[] =
		{0x40420000, // mfc $v0, 0
		  0x03E00008, // jr $ra
		  0x00000000, // nop
                  0x00000000}; // nop

	int (*rawFunc)() = (void*)instr;

// Write coprocessor register number inside instruction		  
	instr[0] |= ( (cop_num & 3) << 26 ) | ( (register_num & 31) << 11 );

// Execute modified instruction
	return rawFunc();
}

unsigned int get_cop0_register(unsigned char register_num)
{
	return get_cop_register(0, register_num);
}

void set_cop_register(unsigned char cop_num,
					unsigned char register_num,
					unsigned int value)
{
// Workaround for MIPS' simplicistic instruction set...	
	unsigned int instr[] =
		{0x40840000, // mtc $a0, 0
		  0x03E00008, // jr $ra
		  0x00000000, // nop
                  0x00000000}; // nop
		  
	void (*rawFunc)(int value) = (void*)instr;

// Write coprocessor register number inside instruction
	instr[0] |= ( (cop_num & 3) << 26 ) | ( (register_num & 31) << 11 );	 

// Execute modified instruction
	rawFunc(value);
}

void set_cop_ctrl_register(unsigned char cop_num,
					unsigned char register_num,
					unsigned int value)
{
// Workaround for MIPS' simplicistic instruction set...	
	unsigned int instr[] =
		{0x40C40000, // ctc $a0, 0
		  0x03E00008, // jr $ra
		  0x00000000, // nop
                  0x00000000}; // nop
		  
	void (*rawFunc)(int value) = (void*)instr;

// Write coprocessor register number inside instruction
	instr[0] |= ( (cop_num & 3) << 26 ) | ( (register_num & 31) << 11 );	 

// Execute modified instruction
	rawFunc(value);
}

void set_cop0_register(unsigned char register_num,
							unsigned int value)
{
	set_cop_register(0, register_num, value);
}

void run_cop_instruction(unsigned char cop_num,
						unsigned int operation)
{
// Workaround for MIPS' simplicistic instruction set...	
	unsigned int instr[] =
		{0x42000000, // cop 0
		  0x03E00008, // jr $ra
		  0x00000000, // nop
                  0x00000000}; // nop
		  
	void (*rawFunc)(void) = (void*)instr;

// Write coprocessor register number inside instruction
	instr[0] |= ( (cop_num & 3) << 26 ) |  (operation & 0x1ffffff);	 

// Execute modified instruction
	rawFunc();
}

