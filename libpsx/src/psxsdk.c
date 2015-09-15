/*
 * PSXSDK Library
 *
 * Free and open source library to develop for the Sony PlayStation
 */
 
#include <psx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exception.h"

#define IPENDING				*((unsigned int*)0x1f801070)
#define IMASK					*((unsigned int*)0x1f801074)
#define RCNT_COUNT(x)			*((unsigned int*)(0x1f801100 + (x<<4)))
#define RCNT_MODE(x)			*((unsigned int*)(0x1f801104 + (x<<4)))
#define RCNT_TARGET(x)		*((unsigned int*)(0x1f801108 + (x<<4)))

const char *sysromver_unavail = "System ROM Version Unavailable";

unsigned char padread_buf[2][8];

void (*vblank_handler_callback)();

extern int *vblank_handler();

void (*rcnt_handler_callback)();

extern int *rcnt_handler();

/*static unsigned int vblank_queue_buf[4] = {0x0, // Will contain next interrupt handler in queue
                                    0x0, // func1
				    (unsigned int)vblank_handler, // func2
				    0x0, // pad
				   };*/
				    
static int vblank_handler_set = 0;				   
static unsigned int vblank_handler_event_id = 0;

static int rcnt_handler_set = 0;
static unsigned int rcnt_handler_event_id = 0;
unsigned int rcnt_handler_evfield;

void _internal_cdromlib_init(void);

static unsigned int psxSdkFlags = 0;

static unsigned char *psxBiosState; 

extern void _96_remove(void);
extern void _96_init(void);
extern void InitCARD(void);
extern void StartCARD(void);
extern void StopCARD(void);
extern void _bu_init(void);
extern void BIOSWarmReboot(void);
				   
void PSX_InitEx(unsigned int flags)
{	
	if(flags & PSX_INIT_NOBIOS)
	{
		printf("Entering No BIOS mode...\n");
		
		__PSX_Init_NoBios();
		goto _initex_end;
	}
	
	if(flags & PSX_INIT_SAVESTATE)
	{
// Save BIOS state
// This simply copies the entire section of RAM used by the BIOS
// in a buffer.		
		EnterCriticalSection();
		psxBiosState = malloc(0x10000);
		memcpy(psxBiosState, (void*)0x80000000, 0x10000);
		ExitCriticalSection();
	}
	
	/* Reinitialize ISO 9660 filesystem driver */
	
	if(flags & PSX_INIT_CD)
	{
		EnterCriticalSection();
		_96_remove();
		ExitCriticalSection();
	
		_96_init();
	}
	
	
	/*This is needed, otherwise PSX will crash when VBlank handler is set*/
	/*InitCARD(1);
	StartCARD();
        StopCARD();*/

	if(flags & PSX_INIT_CD)
		_internal_cdromlib_init();

	printf("PSXSDK testing version !!!\n");
	
	vblank_handler_set = 0;
_initex_end:
	psxSdkFlags = flags;
}

void PSX_Init(void)
{
	PSX_InitEx(PSX_INIT_CD);
}

void PSX_DeInit(void)
{
	if(psxSdkFlags & PSX_INIT_CD)
	{
		EnterCriticalSection();
		_96_remove();
		ExitCriticalSection();
	}
	
	RemoveVBlankHandler();
	
	if(psxSdkFlags & PSX_INIT_SAVESTATE)// This must always be the last to be called!
		PSX_RestoreBiosState();
}

void PSX_ReadPad(unsigned short *padbuf, unsigned short *padbuf2)
{
	int x;
	unsigned char arr[PAD_READ_RAW_SIZE];
	unsigned short *padbuf_a[2];
	
// Now uses low level pad routines...	
	padbuf_a[0] = padbuf;
	padbuf_a[1] = padbuf2;
		
	for(x = 0; x < 2; x++)
	{
		pad_read_raw(x, arr);
		
		if(arr[2] == 0x5a)
		{
			*padbuf_a[x] = (arr[3]<<8)|arr[4];
			*padbuf_a[x] = ~*padbuf_a[x];
		}
		else
			*padbuf_a[x] = 0;
	}
}

unsigned char psxsdkPadArr[PAD_READ_RAW_SIZE];

void PSX_PollPad(int pad_num, psx_pad_state *pad_state)
{
//	int x;
	/*unsigned short *padbuf_a[2];
	
// Now uses low level pad routines...	
	padbuf_a[0] = padbuf;
	padbuf_a[1] = padbuf2;
		
	for(x = 0; x < 2; x++)
	{
		pad_read_raw(x, arr);
		
		if(arr[2] == 0x5a)
		{
			*padbuf_a[x] = (arr[3]<<8)|arr[4];
			*padbuf_a[x] = ~*padbuf_a[x];
		}
		else
			*padbuf_a[x] = 0;
	}*/
	
	unsigned char *arr = psxsdkPadArr;
	
	pad_read_raw(pad_num, arr);
	
	pad_state->status = arr[0];
	pad_state->id = arr[1];
	
	pad_state->buttons = (arr[3]<<8)|arr[4];
	pad_state->buttons = ~pad_state->buttons;
	
	switch(pad_state->id)
	{
		case 0xFF:
			pad_state->type = PADTYPE_NONE;
		break;
		case 0x41:
			pad_state->type = PADTYPE_NORMALPAD;
		break;
		case 0x53:
			pad_state->type = PADTYPE_ANALOGJOY;
			pad_state->extra.analogJoy.x[0] = arr[5]-128;
			pad_state->extra.analogJoy.y[0] = arr[6]-128;
			pad_state->extra.analogJoy.x[1] = arr[7]-128;
			pad_state->extra.analogJoy.y[1] = arr[8]-128;
		break;
		case 0x73:
			pad_state->type = PADTYPE_ANALOGPAD;
			pad_state->extra.analogPad.x[0] = arr[5]-128;
			pad_state->extra.analogPad.y[0] = arr[6]-128;
			pad_state->extra.analogPad.x[1] = arr[7]-128;
			pad_state->extra.analogPad.y[1] = arr[8]-128;
		break;
		case 0x23:
			pad_state->type = PADTYPE_NEGCON;
			pad_state->extra.negCon.steering = arr[5]-128;
			pad_state->extra.negCon.one = arr[6];
			pad_state->extra.negCon.two = arr[7];
			pad_state->extra.negCon.shoulder = arr[8];
		break;
		case 0x31:
			pad_state->type = PADTYPE_KONAMIGUN;
		break;
		default:
			pad_state->type = PADTYPE_UNKNOWN;
	}
}

/*int PSX_GetPadType(unsigned int pad_num)
//{	
	//#warning "Function does not currently work!"
//	unsigned char arr[16];
	
	pad_read_raw(pad_num, arr);

	switch(arr[1])
	{
		case 0xFF:
			return PADTYPE_NONE;
		break;
		case 0x41:
			return PADTYPE_NORMALPAD;
		break;
		case 0x53:
			return PADTYPE_ANALOGJOY;
		break;
		case 0x73:
			return PADTYPE_ANALOGPAD;
		break;
	}
	
	return PADTYPE_UNKNOWN;
}*/


void PSX_GetSysInfo(struct psx_info *info)
{
	unsigned long i,i2;

	info->kernel.version = GetKernelRomVersion();
	
	i = GetKernelDate();
	
/*
 * Convert year from BCD to decimal
 */
	
	i2 = i >> 16;
	
	info->kernel.year = i2 & 0xf;
	info->kernel.year+= ((i2>>4)&0xf)*10;
	info->kernel.year+= ((i2>>8)&0xf)*100;
	info->kernel.year+= ((i2>>12)&0xf)*1000;
	
/*
 * Convert month from BCD to decimal
 */
	i2 = (i >> 8) & 0xff;
	
	info->kernel.month = i2 & 0xf;
	info->kernel.month+= (i2>>4) * 10;	
		
/*
 * Convert day from BCD to decimal
 */
	i2 = i & 0xff;
	
	info->kernel.day = i2 & 0xf;
	info->kernel.day+= (i2>>4) * 10;

/*
 * Unless we receive something in the range >= 1 && <= 16,
 * RAM size will be reported as 2 Megabytes
 */
 
	i = GetRamSize();
	
	if(i == 0 || i > 16)
		info->system.memory = 2<<20; /* 2 Megabytes */
	else
		info->system.memory <<= 20;
}
	


int get_real_file_size(char *name)
{
	struct DIRENTRY dirent_buf;
	
	if(firstfile(name, &dirent_buf) == &dirent_buf)
		return dirent_buf.size;
	else
		return 0;
}

int get_file_size(char *name)
{
	int i = get_real_file_size(name);
	
	if(strncmp(name, "cdrom:", 6) == 0)
	{
		if(i & 0x7ff)
		{
			i += 0x800;
			i &= ~0x7ff;
		}
	}else if(strncmp(name, "bu", 2) == 0)
	{
		if(i & 0x7f)
		{
			i += 0x80;
			i &= ~0x7f;
		}
	}
	
	return i;
}

int SetRCnt(int spec, unsigned short target, unsigned int mode)
{
	spec &= 0xf;
	
	if(spec >= 3)
		return 0;
	
	RCNT_MODE(spec)=0;
	RCNT_TARGET(spec)=target;
	RCNT_MODE(spec)=mode;
	
	return 1;
}

int GetRCnt(int spec)
{
	spec &= 0xf;
	
	if(spec >= 4)
		return -1;
	
	return (RCNT_COUNT(spec) & 0xffff);
}

int StartRCnt(int spec)
{
	spec &= 0xf;
	
	if(spec >= 3)
		return 0;
	
	IMASK |= 1 << (spec + 4);
	
	return 1;
}

int StopRCnt(int spec)
{
	spec &= 0xf;
	
	if(spec >= 3)
		return 0;
	
	IMASK ^= 1 << (spec + 4);
	
	return 1;
}
	
void SetVBlankHandler(void (*callback)())
{
	if(psxSdkFlags & PSX_INIT_NOBIOS)
	{
		_EXC_vblank_handler_set = 0;
		_EXC_vblank_handler = callback;
		_EXC_vblank_handler_set = 1;
		return;
	}
		
	if(vblank_handler_set == 1)
	{
		EnterCriticalSection();
		
		vblank_handler_callback = callback;
	
		ExitCriticalSection();
		
		return;
	}
	
// Enter critical section
	
	EnterCriticalSection();
	
	IMASK|=1;
	
	vblank_handler_event_id = OpenEvent(RCntCNT3, 2, 0x1000, vblank_handler);
	EnableEvent(vblank_handler_event_id);
	
	vblank_handler_callback = callback;
	vblank_handler_set = 1;

// Exit critical section
	
	ExitCriticalSection();
}

void RemoveVBlankHandler(void)
{
	if(psxSdkFlags & PSX_INIT_NOBIOS)
	{
		_EXC_vblank_handler_set = 0;
		_EXC_vblank_handler = NULL;
		return;
	}
		
	if(vblank_handler_set)
	{
		EnterCriticalSection();
		
		DisableEvent(vblank_handler_event_id);
		CloseEvent(vblank_handler_event_id);
	
		//IMASK^=1;
		// ^ commented because masking out vblank could give problems to other bios functions
	
		vblank_handler_set = 0;
		
		ExitCriticalSection();
	}
}

void SetRCntHandler(void (*callback)(), int spec, unsigned short target)
{
	if(psxSdkFlags & PSX_INIT_NOBIOS)
		return; // Not yet supported in No-Bios Mode
	
	if(rcnt_handler_set)
	{
		EnterCriticalSection();
		
		rcnt_handler_callback = callback;
		
		ExitCriticalSection();
		
		return;
	}
	
// Enter critical section
	
	SetRCnt(spec, target, RCntIntr | 0x08 | 0x10 | 0x40);
	StartRCnt(spec);
	
	EnterCriticalSection();
	rcnt_handler_event_id = OpenEvent(spec, 2, 0x1000, rcnt_handler);
	EnableEvent(rcnt_handler_event_id);
	
	rcnt_handler_callback = callback;
	rcnt_handler_set = spec;

	switch(spec)
	{
		case RCntCNT0: rcnt_handler_evfield = 1 << 4; break;
		case RCntCNT1: rcnt_handler_evfield = 1 << 5; break;
		case RCntCNT2: rcnt_handler_evfield = 1 << 6; break;
		case RCntCNT3: rcnt_handler_evfield = 1; break;
	}
	
// Exit critical section
	
	ExitCriticalSection();
}

void RemoveRCntHandler(int spec)
{
	if(psxSdkFlags & PSX_INIT_NOBIOS)
		return; // Not yet supported in No-Bios Mode
	
	if(rcnt_handler_set)
	{
		EnterCriticalSection();
		
		DisableEvent(rcnt_handler_event_id);
		CloseEvent(rcnt_handler_event_id);
		
		rcnt_handler_set = 0;
		
		ExitCriticalSection();
	}
}

const char *GetSystemRomVersion(void)
{
// Get pointer to zero-terminated string containing System ROM Version which is embedded in
// most PlayStation BIOSes.

// If getting the pointer is not possible, a pointer to a string saying "System ROM Unavailable" is returned.
	
	int x;
	
	for(x = 0x7ffee; x >= 0; x--)
		if(memcmp("System ROM Version", (void*)(0xbfc00000 + x), 18) == 0)
			return (char*)(0xbfc00000 + x);
	
	return sysromver_unavail;
}

int PSX_RestoreBiosState(void)
{
	if(!(psxSdkFlags & PSX_INIT_SAVESTATE))
		return 0; // can't restore BIOS state if it was not saved previously
	
	EnterCriticalSection();
	memcpy((void*)0x80000000, psxBiosState, 0x10000);
	ExitCriticalSection();
	
	return 1;
}

unsigned int PSX_GetInitFlags(void)
{
	return psxSdkFlags;
}

void PSX_WarmReboot(void)
{
	if(psxSdkFlags & PSX_INIT_NOBIOS)
	{
psx_warmreboot_nobios:
		PSX_DeInit();
		__asm__("j _start");
		__asm__("nop");
	}
	else
	{
		if(!(psxSdkFlags & PSX_INIT_CD))
			goto psx_warmreboot_nobios; 
		
		BIOSWarmReboot();
	}
}
