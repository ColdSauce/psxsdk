/*
 * PSXSDK Memory Card Helper Functions
 *
 * These functions help to manage memory card loading/saving
 *
 * Normal file functions can be used to do this, but it will be very tedious...
 */
 
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <fcntl.h>
 #include <memcard.h>
 #include <string.h>
 #include <psx.h>

static unsigned char card_cmd[140];
static unsigned char arr[140];

unsigned char McReadSector(int card_slot, int sector, unsigned char *buffer)
{	
	memset(&card_cmd[0], 0, 140);
	
	card_cmd[0] = 0x81;		/*MC access*/
	card_cmd[1] = 0x52;		/*Read command*/
	
	/*Copy frame number to command*/
	card_cmd[4] = sector >> 8;			/*Frame MSB*/
	card_cmd[5] = sector & 0xFF;			/*Frame LSB*/
	
	QueryPAD(card_slot, card_cmd, arr, sizeof(card_cmd));

	/*Copy received frame data*/
	memcpy(buffer, &arr[10], 128);
	
	/*Return RW status*/
	return arr[139];
}

unsigned char McWriteSector(int card_slot, int sector, unsigned char *buffer)
{
	int i;
	
	memset(&card_cmd[0], 0, 140);
	
	card_cmd[0] = 0x81;		/*MC access*/
	card_cmd[1] = 0x57;		/*Write command*/
	
	/*Copy frame number to command*/
	card_cmd[4] = sector >> 8;			/*Frame MSB*/
	card_cmd[5] = sector & 0xFF;			/*Frame LSB*/
	
	memcpy(&card_cmd[6], buffer, 128);

	/* Compute checksum */
	for(i = 4, card_cmd[134] = 0; i < 134; i++)
		card_cmd[134] ^= card_cmd[i];
	
	QueryPAD(card_slot, card_cmd, arr, sizeof(card_cmd));

	/*Return RW status*/
	return arr[137];
}

unsigned int McGetStatus(int card_slot)
{
	unsigned int status = 0;
	
	memset(&card_cmd[0], 0, 140);
	
	card_cmd[0] = 0x81;		/*MC access*/
	card_cmd[1] = 0x52;		/*Read command*/
	
	/*Copy frame number to command*/
	card_cmd[4] = 0;//sector >> 8;			/*Frame MSB*/
	card_cmd[5] = 0;//sector & 0xFF;			/*Frame LSB*/
	
	QueryPAD(card_slot, card_cmd, arr, sizeof(card_cmd));

	if(arr[2] == 0x5a && arr[3] == 0x5d)
		status |= MEMCARD_CONNECTED;
	
	if(arr[6] == 'M' && arr[7] == 'C')
		status |= MEMCARD_FORMATTED;
	
	return status;
}
