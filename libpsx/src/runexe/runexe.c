/**
 * PSXSDK
 *
 * RunEXE functionality
 */

#include <psx.h>
#include <stdio.h>
#include <string.h>
#include <runexe.h>

// Include runexe stage2 position independent code

extern void PSX_RunExeStage2();
extern void PSX_RunExeStage2_END();

static void *runExeBuffer;

int PSX_RunExe(void *exeBuffer)
{
	//unsigned char *exe = exeBuffer;
	unsigned int *exe = exeBuffer;
// Size of position indipendent code	
	int sz = PSX_RunExeStage2_END -
			PSX_RunExeStage2;
	
	if(!(PSX_GetInitFlags() & PSX_INIT_SAVESTATE))
	{
		printf("RunExe error: State was not saved!\n");
		return 0;
	}
	
	if(((unsigned int)exeBuffer & 3))
	{
		printf("RunExe: Alignment violation.\n");
		printf("Specified buffer must have an address which is a multiplier of 4.\n");
		return 0;
	}
	
	// Check magic string
	if(strcmp(exeBuffer, "PS-X EXE") != 0)
		return 0; // Return failure if magic string couldn't be found
	
	runExeBuffer = exeBuffer;
	
	printf("PSXSDK RunExe Debug Information\n");
	printf("t_addr = %08x\n", exe[6]);
	printf("t_size = %08x\n", exe[7]);
	printf("d_addr = %08x\n", exe[8]);
	printf("d_size = %08x\n", exe[9]);
	printf("b_addr = %08x\n", exe[10]);
	printf("b_size = %08x\n", exe[11]);
	printf("s_addr = %08x\n", exe[12]);
	printf("s_size = %08x\n", exe[13]);
	
	printf("OK..! Farewell.\n");

// Deinitialize the PSXSDK library	
	PSX_DeInit();
	
// Copy the position independent stage2 code in high memory
	memcpy((void*)0x801fef00, PSX_RunExeStage2, sz);
	
// Stack is unusable now! Do not do anything else except calling the stage2 code.
	
// Jump to the stage2 code without using the stack..
	
	__asm__("la $a0, runExeBuffer");
	__asm__("nop");
	__asm__("lw $a0, 0($a0)");
	__asm__("j 0x801fef00");
	__asm__("nop");
	
// This is never reached but keep the compiler happy	
	return 0;
}
