#ifndef _PSXSDK_MEMCARD_H
#define _PSXSDK_MEMCARD_H

/*Card connection related*/
enum memcard_status
{
	/** Memory card is connected */
	MEMCARD_CONNECTED = 1,
	/** Memory card is formatted */
	MEMCARD_FORMATTED = 2
};

/**
 * Results of memory card operations
 */
 
enum memcard_operation_result
{
	/** Operation was successful - good result */
	MEMCARD_RW_GOOD = 0x47,
	/** Bad checksum - bad result */
	MEMCARD_RW_BADCHECK	 = 0x4E,
	/** Bad sector - bad result */
	MEMCARD_RW_BADSECTOR = 0xFF
};

/*Card block type*/
/*#define BLOCK_FORMATTED 		0x000000A0
#define BLOCK_INITIAL			0x00000051
#define BLOCK_MIDLINK			0x00000052
#define BLOCK_ENDLINK			0x00000053
#define BLOCK_DEL_INITIAL		0x000000A1
#define BLOCK_DEL_MIDLINK		0x000000A2
#define BLOCK_DEL_ENDLINK		0x000000A3*/

/*128 byte card directory*/
/*typedef struct{
	long BlockType;
	long FileSize;
	short NextBlock;
	char FileName[20];
	char Reserved[97];
	char XorChecksum;
}CARDDIR;*/

/**
 * Reads a 128-byte sector from a memory card.
 * @param card_slot Memory card slot (0 = first slot, 1 = second slot)
 * @param sector Sector number (0-511)
 * @param buffer Pointer to data buffer in which data will be stored
 * @return Result of operation (possible values in memcard_operation_result enum)
 */
 
unsigned char McReadSector(int card_slot, int sector, unsigned char *buffer);

/**
 * Writes a 128-byte sector to a memory card.
 * @param card_slot Memory card slot (0 = first slot, 1 = second slot)
 * @param sector Sector number (0-511)
 * @param buffer Pointer to data buffer containing data to write
 * @return Result of operation (possible values in memcard_operation_result enum)
 */

unsigned char McWriteSector(int card_slot, int sector, unsigned char *buffer);

/**
 * Get memory card status
 * @param card_slot Memory card slot (0 = first slot, 1 = second slot)
 * @return Bitmask for current memory card status (flags in memcard_status enum)
 */

unsigned int McGetStatus(int card_slot);

#endif
