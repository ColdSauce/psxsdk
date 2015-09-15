#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <psx.h>

/*
 * Flags taken from PCSX
 */

// Status Flags
#define TX_RDY		0x0001
#define RX_RDY		0x0002
#define TX_EMPTY	0x0004
#define PARITY_ERR	0x0008
#define RX_OVERRUN	0x0010
#define FRAMING_ERR	0x0020
#define SYNC_DETECT	0x0040
#define DSR			0x0080
#define CTS			0x0100
#define IRQ			0x0200

// Control Flags
#define TX_PERM		0x0001
#define DTR			0x0002
#define RX_PERM		0x0004
#define BREAK		0x0008
#define RESET_ERR	0x0010
#define RTS			0x0020
#define PADSIO_RESET	0x0040

/*
    from BlackBag/Nagra PSX

    0x1f801040 - unsigned char data;
    0x1f801044 - unsigned short status;
    0x1f80104a - unsigned short cntl;
    0x1f80104e - unsigned short baud;
*/

#define PADSIO_DATA(x)	*((unsigned char*)(0x1f801040 + (x<<4)))
#define PADSIO_STATUS(x)	*((unsigned short*)(0x1f801044 + (x<<4)))
#define PADSIO_MODE(x)	*((unsigned short*)(0x1f801048 + (x<<4)))
#define PADSIO_CTRL(x)	*((unsigned short*)(0x1f80104a + (x<<4)))
#define PADSIO_BAUD(x)	*((unsigned short*)(0x1f80104e + (x<<4)))

unsigned char readpad_vibrations[4][2];
int querypad_rxrdy = 1;

void QueryPAD(int pad_n, unsigned char *in, unsigned char *out, int len)
{
	int x;
	int y;
	int i;
	unsigned char TempData;
	int EmuFlag = 0;
	
	PADSIO_MODE(0) = 0xD;
	PADSIO_BAUD(0) = 0x88;
	
	if(pad_n == 1) PADSIO_CTRL(0) = 0x3003; else PADSIO_CTRL(0) = 0x1003;
	
	/*Get the initial command (usually 0x01 or 0x81)*/
	TempData = *in;

	for(y=0;y<400;y++);	/*Slight delay before first transmission*/

	for(x = 0; x < len; x++)
	{
		/*Must use timeouts or else program hangs on emulators*/
		if(!EmuFlag)
		{
			for(y=0;y<1000;y++)
			{
				/*Wait for TX ready*/
				if(PADSIO_STATUS(0) & 4)break;
			}
		}
		
		PADSIO_DATA(0) = *in;
		in++;

		if(!EmuFlag)
		{
			/*Check ACK only for Memory Cards*/
			if(TempData == 0x81 || x == 0)
			{
				for(y=0;y<2000;y++)
				{
					/*Check for ACK signal*/
					if(PADSIO_STATUS(0) & 128)break;
				}
			}
			
			for(i=0;i<100;i++)
			{
				/*Read RX status flag, required for Xebra*/
				if(PADSIO_STATUS(0) & 2)break;
			}
		}
		
		*out = PADSIO_DATA(0);
		
		/*This is emulator, valid data was received without ACK, ePSXe and PCSX*/
		if(x == 0 && y > 1900 && *out != 0xFF)EmuFlag = 1;
		
		out++;
	}
	
	PADSIO_CTRL(0) = 0;
}

void pad_read_raw(int pad_n, unsigned char *arr)
{
	// arr must be at least 16 bytes long...
	
	unsigned char pad_cmd[PAD_READ_RAW_SIZE] = {1,0x42,0,0,0};
	
	pad_cmd[3] = readpad_vibrations[pad_n][0];
	pad_cmd[4] = readpad_vibrations[pad_n][1];
	
	QueryPAD(pad_n, pad_cmd, arr, sizeof(pad_cmd));
}

void pad_escape_mode(int pad_n, int enable)
{
	unsigned char pad_cmd[] = {1,0x43,0,enable?1:0,0};
	
	QueryPAD(pad_n, pad_cmd, NULL, sizeof(pad_cmd));
}

void pad_enable_vibration(int pad_n)
{
	unsigned char pad_cmd[] = {1, 0x4d, 0, 0, 1, 0xff, 0xff, 0xff, 0xff};
	
	pad_escape_mode(pad_n, 1); // Enter escape / configuration mode
	QueryPAD(pad_n, pad_cmd, NULL, sizeof(pad_cmd));
	pad_escape_mode(pad_n, 0); // Exit escape / configuration mode
}

void pad_set_vibration(int pad_n, unsigned char small, unsigned char big)
{
	if(pad_n >= 0 && pad_n <= 3)
	{
		readpad_vibrations[pad_n][0] = small;
		readpad_vibrations[pad_n][1] = big;
	}
}

/*
 // Sony pads make this interface unpractical!
void pad_set_analog(int pad_n, int lock)
{
	unsigned char pad_cmd[9] =
		{1, 0x44, 0, 1, lock?3:0, 0, 0, 0, 0};
	unsigned char pad_cmd2[9] =
		{1, 0x4f, 0, 0xff, 0xff, 3, 0, 0, 0};
	
	pad_escape_mode(pad_n, 1);
	QueryPAD(pad_n, pad_cmd, NULL, sizeof(pad_cmd));
	QueryPAD(pad_n, pad_cmd2, NULL, sizeof(pad_cmd));	
	pad_escape_mode(pad_n, 0);
}*/

