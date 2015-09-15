/*
 * PSXSDK Library include
 */
 
#ifndef _PSX_H
#define _PSX_H

#include <stdarg.h>

#ifndef true
#define true 1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef false
#define false 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

/**
 * PSXSDK version information in hexadecimal format.
 *
 * An explanation of version numbers and how they relate to
 * releases:
 * + PSXSDK_VERSION undefined
 *   - PSXSDK 0.1
 * + 0.1.99 (0x0199)
 *   -  From PSXSDK releases 2012-03-03 up to 2013-01-14.
 * + 0.2.99 (0x0299)
 *   - PSXSDK 2013-05-14
 * + 0.3.99 (0x0399)
 *   - PSXSDK 2013-11-09
 * + 0.4.99 (0x0499)
 *   - PSXSDK 2014-04-22
 * + 0.5.99 (0x0599)
 *   - PSXSDK 2015-07-29
 */

#define PSXSDK_VERSION			0x0599

/**
 * PSXSDK version information in string format
 */
 
#define PSXSDK_VERSION_STRING		"0.5.99"

/**
 * PSXSDK version date (BCD YYYY-MM-DD)
 */

#define PSXSDK_VERSION_DATE		0x20150729


/*
 * Include some GCC builtin includes
 */

#ifndef _PSXSDK_WRAPPER

#include <psxbios.h>

#endif

#include <psxgpu.h>
#include <memcard.h>

#include <psxpad.h>
#include <psxspu.h>
#include <psxcdrom.h>
#include <psxsio.h>
//#include <adpcm.h>
#include <psxgte.h>

/**
 * Scratch pad - unused data cache, that can be used as "fast RAM"
 */

extern unsigned char __scratchpad[1024];

/**
 * Coprocessor 0 register numbers
 */

enum cop0_register_numbers
{
	/** Contains the last invalid program address which caused a trap. 
	  
	      It is set by address errors of all kinds. */
	COP0_BADVADDR = 8,
	/** CPU mode flags (status register) */
	COP0_SR = 12,
	/** Describes the most recently recognized exception. */
	COP0_CAUSE = 13,
	/** Return address from trap */
	COP0_EPC = 14,
	/** COP0 type and revision level */
	COP0_PRID = 15,
};

/**
 * Root counter specifications
 */

enum psx_rcnt_specs
{
	/** Pixel clock*/
	RCntCNT0		= 0xf2000000,
	/** Horizontal sync*/
	RCntCNT1		= 0xf2000001,
	/** System clock / 8 */
	RCntCNT2		= 0xf2000002,
	/** VSync (VBlank) */
	RCntCNT3		= 0xf2000003,
};
	
/**
 * Root counter modes
 */

enum psx_rcnt_modes
{
	/** Interrupt mode */
	RCntIntr =	0x1000,
	/** Ignore target and count to 65535 (hex: 0xFFFF) */
	RCntNotar =	0x0100,
	/** Timer stop mode */
	RCntStop =	0x0010,
	 /** System Clock mode */
	RCntSC =		0x0001,
};

struct psx_info
{
	struct kernel
	{
		const char *version; // Kernel version
		int year; // Kernel year
		int month; // Kernel month
		int day; // Kernel day
	}kernel;
	
	struct system
	{
		int memory; // RAM memory size
	}system;
};

/**
 * Initialize library
 */

void PSX_Init(void);

/**
 * Flags for PSX_Init.
 */

enum psx_init_flags
{
	/** PSX_INIT_CD - Initialize CDROM filesystem */
	PSX_INIT_CD = 1,
	/** PSX_INIT_SAVESTATE - Save BIOS state before initializing the library */
	PSX_INIT_SAVESTATE = 2,
	/** PSX_INIT_NOBIOS - Remove control from the BIOS and let PSXSDK be in complete control */
	PSX_INIT_NOBIOS = 4,
};

/**
 * Deinitialize library
 */

void PSX_DeInit(void);

/**
 * Initialize library (extended version)
 *
 * flags contains a bitmask specifying which flags are enabled
 *
 * PSX_Init() is the same as PSX_InitEx(PSX_INIT_CD)
 * @param flags Flag bitmask (flags are to be OR'd)
 */

void PSX_InitEx(unsigned int flags);

/**
 * Reads the status of the buttons of the two joypads.
 *
 * Takes two pointers for 16-bit bitmasks, one for the first player's joypad,
 * and one for the second one. Their bits are updated to show which buttons were pressed.
 *
 * This function only supplies basic functionality, adequate for a normal digital pad.
 *
 * If more advanced functionality is desired, use PSX_PollPad() instead of this function.
 * @attention Note that some joypads, like the official ones from Sony, do not like to be polled more than 
 * once every 1/60th of a second and if this limitation is not considered the data
 * they return is undefined. Other joypads do not have this limitation but in any case err on the safe side. 
 * @param padbuf Pointer to 16-bit variable where bitmask for pad #1 will be stored.
 *                           If NULL is passed, this argument is ignored.
 * @param padbuf2 Pointer to 16-bit variable where bitmask for pad #2 will be stored
 *                           If NULL is passed, this argument is ignored.
 */
void PSX_ReadPad(unsigned short *padbuf, unsigned short *padbuf2);

/**
 * Polls a joypad for information.
 * @attention Note that some joypads, like the official ones from Sony, do not like to be polled more than 
 * once every 1/60th of a second and if this limitation is not considered the data
 * they return is undefined. Other joypads do not have this limitation but in any case err on the safe side. 
 * @param pad_num Number of the pad to poll (0 = pad #1, 1 = pad #2, etc.)
 * @param pad_state Pointer to a psx_pad_state structure in which to store information for the pad.
 */
 
void PSX_PollPad(int pad_num, psx_pad_state *pad_state);

/**
 * Takes a pointer to a struct psx_info structure, and fills it
 * with information about the PlayStation on which the program is running.
 * PS-OS kernel build date and version are reported among other things.
 * @param info Pointer to struct psx_info structure which will be filled.
 */
void PSX_GetSysInfo(struct psx_info *info);

/**
 * Gets Coprocessor 0 status register
 * @return Value of Coprocessor 0 status register
 */
 
unsigned int get_cop0_status(void);

/**
 * Sets Coprocessor 0 status register
 * @param sreg New value of Coprocessor 0 status register
 * @return [[ check: maybe this is a void function ]]
 */

unsigned int set_cop0_status(unsigned int sreg);

/**
 * Gets the contents of the program counter when the 
 * last exception happened.
 * @return Value of the program counter at the time of the last exception
 */
 
unsigned int get_cop0_epc(void);

/**
 * Get value of specified Coprocessor 0 register
 * @param register_num Number of Coprocessor 0 register whose value must be retrieved
 * @return Value of specified Coprocessor 0 register
 */

unsigned int get_cop0_register(unsigned char register_num);

/**
 * Set value of specified Coprocessor 0 register
 * @param register_num Number of Coprocessor 0 register whose value must be set
 * @param value New value of specified Coprocessor 0 register
 */

void set_cop0_register(unsigned char register_num, unsigned int value);

/**
 * Get value of the specified (data) register of a specified coprocessor
 * @param cop_num Coprocessor number 
 * @param register_num Number of coprocessor register whose value must be retrieved
 * @return Value of specified coprocessor register
 */
 
unsigned int get_cop_register(unsigned char cop_num,
							unsigned char register_num);


/**
 * Get value of the specified control register of a specified coprocessor
 * @param cop_num Coprocessor number 
 * @param register_num Number of coprocessor register whose value must be retrieved
 * @return Value of specified coprocessor register
 */
 
unsigned int get_cop_ctrl_register(unsigned char cop_num,
							unsigned char register_num);

/**
 * Set value of the specified (data) register of a specified coprocessor
 * @param cop_num Coprocessor number
 * @param register_num Number of Coprocessor 0 register whose value must be set
 * @param value New value of specified Coprocessor 0 register
 */
 
 void set_cop_register(unsigned char cop_num,
				unsigned char register_num, unsigned int value);

/**
 * Set value of the specified control register of a specified coprocessor
 * @param cop_num Coprocessor number
 * @param register_num Number of Coprocessor 0 register whose value must be set
 * @param value New value of specified Coprocessor 0 register
 */
 
 void set_cop_ctrl_register(unsigned char cop_num,
				unsigned char register_num, unsigned int value);

/**
 * Make the specified coprocessor run the specified instruction
 * @param operation Operation number for the instruction
 */

void run_cop_instruction(unsigned char cop_num,
						unsigned int operation);
 
// Root counter functions

/**
 * Set root counter (documentation TO DO)
 * @param spec Spec
 * @param target Target
 * @param mode mode
 */

int SetRCnt(int spec, unsigned short target, unsigned int mode);

/**
 * Get root counter (documentation TO DO)
 * @param spec Spec
 * @return TO DO
 */

int GetRCnt(int spec);

/**
 * Start root counter (documentation TO DO)
 * @param spec Spec
 * @return TO DO
 */

int StartRCnt(int spec);

/**
 * Stop root counter (documentation TO DO)
 * @param spec Spec
 * @return TO DO
 */

int StopRCnt(int spec);

/**
 * Restores BIOS state to the one prior the initialization of the library.
 * This function can only be used if PSX_InitEx() was called with the
 * PSX_INIT_SAVESTATE flag.
 * It is usually called by PSX_DeInit() automatically, so unless
 * you know what you are doing, you do not need to call it yourself.
 * @return 1 on success, 0 on failure, such as the fact that PSX_InitEx() wasn't called
 * with the PSX_INIT_SAVESTATE flag.
 */

int PSX_RestoreBiosState(void);

/**
 * Gets the bitmask for the flags passed to PSX_InitEx()
 * @return Flag bitmask
 */
 
unsigned int PSX_GetInitFlags(void);

/**
 * Sets an handler function for the VBlank interrupt.
 * Used for simple, inaccurate timing - the handler function gets called 60 times a second
 * in NTSC video mode and 50 times a second in PAL video mode.
 *
 * While most games use the VBlank interrupt for timing as they don't require high precision and 
 * have mechanisms to keep up with the different speed in PAL or NTSC video mode, for precise
 * timing VBlank is inadequate. It is better to look at root counters if you desire precision.
 *
 * If there is already a VBlank handler set, this function replaces the current handler with the specified one.
 * @param h Pointer to (new) VBlank handler function
 */

void SetVBlankHandler(void (*h)());

/**
 * Removes a previously set VBlank handler.
 *
 * If SetVBlankHandler() was not called before, calling this function has no effect.
 */

void RemoveVBlankHandler(void);

#endif
