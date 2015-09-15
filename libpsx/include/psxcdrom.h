#ifndef _PSXCDROM_H
#define _PSXCDROM_H

#define CDSTATUS_PLAY			0x80
#define CDSTATUS_SEEK			0x40
#define CDSTATUS_SHELLOPEN	0x10

// Command names

enum
{
	CdlSync = 0, 
	CdlNop = 1,  CdlGetstat = 1,
	CdlSetloc = 2, 
	CdlPlay = 3,
	CdlForward = 4, 
	CdlBackward = 5, 
	CdlReadN = 6, 
	CdlStandby = 7,
	CdlStop = 8,
	CdlPause = 9, 
	CdlInit = 10, 
	CdlMute = 11,
	CdlDemute = 12,
	CdlSetfilter = 13,
	CdlSetmode = 14,
	CdlSetparam = 15,
	CdlGetlocL = 16,
	CdlGetlocP = 17, 
	CdlCmd18 = 18,
	CdlGetTN = 19,
	CdlGetTD = 20, 
	CdlSeekL = 21, 
	CdlSeekP = 22, 
	CdlCmd23 = 23,
	CdlCmd24 = 24, 
	CdlTest = 25, 
	CdlID = 26,
	CdlReadS = 27,
	CdlReset = 28, 
	CdlCmd29 = 29, 
	CdlReadTOC = 30
};

/*
 * Send a low-level CDROM command
 * cmd = command number
 * num = number of arguments
 * ... = arguments
 */

void CdSendCommand(int cmd, int num, ...);

/**
 * Reads the results of a low-level CDROM command
 *
 * @param out Pointer to array of chars where the output will be stored
 * @param max Maximum number of bytes to store
 *
 * Return value: number of results.
 */

int CdReadResults(unsigned char *out, int max);

/**
 * Gets CDROM drive status
 * @return CDROM drive status bitmask
 */

int CdGetStatus(void);

/**
 * Play an Audio CD track
 * @return 1 on success, 0 on failure
 */

int CdPlayTrack(unsigned int track);

unsigned char CdRamRead(unsigned short addr);


#endif
