#ifndef _PSXSIO_H
#define _PSXSIO_H

/** Bitrate reload factors */

enum sio_reload_factors
{
	/** STOP */
	SIO_REL_STOP = 0,
	/** MUL1 */
	SIO_REL_MUL1 = 1,
	/** MUL16 */
	SIO_REL_MUL16 = 2,
	/** MUL64 */
	SIO_REL_MUL64 = 3
};
	
/** Character (data) length settings */

enum sio_data_len
{
	/** Data Length = 5 bits */
	SIO_DATA_LEN_5 = 0,
	/** Data Length = 6 bits */
	SIO_DATA_LEN_6 = 1,
	/** Data Length = 7 bits */
	SIO_DATA_LEN_7 = 2,
	/** Data Length = 8 bits */
	SIO_DATA_LEN_8 = 3
};

/** Stop bit length settings */

enum sio_stop_bit
{
	/** Length = 1 bit */
	SIO_STOP_BIT_1 = 1,
	/** Length = 1.5 bits */
	SIO_STOP_BIT_1_5 = 2,
	/** Length = 2 bits */
	SIO_STOP_BIT_2 = 3
};
	
/** Parity settings */

enum sio_parity
{
	/** No parity */
	SIO_PARITY_NONE =	0,
	/** Odd parity */
	SIO_PARITY_ODD = 1,
	/** Even parity */
	SIO_PARITY_EVEN = 3
};

/** SIO FIFO Buffer (TX/RX) Register [Read/Write] */
#define SIO_TX_RX		*((volatile unsigned char*)0x1F801050)
/** SIO Status Register [Read Only] */
#define SIO_STAT		*((volatile unsigned short*)0x1F801054)
/** SIO Mode Register [Read/Write] */
#define SIO_MODE		*((volatile unsigned short*)0x1F801058)
/** SIO Control Register [Read/Write] */
#define SIO_CTRL		*((volatile unsigned short*)0x1F80105A)
/** SIO Baud Rate Register [Read/Write] */
#define SIO_BPSV		*((volatile unsigned short*)0x1F80105E)


/**
 * Initialize SIO communication at the specified bitrate (baud rate).
 * Mode is 8N1. (Data Length = 8 bit, No parity, Stop bit Length = 1 bit)
 * @param bitrate Bitrate (baud rate)
 */
void SIOStart(int bitrate);

/**
 * Same as SIOStart() but with more control.
 * IMPORTANT: Must use defined macros.
 * For example setting datalength to 5 should be done with "SIO_DATA_LEN_5"
 * and not by simply passing 5 as an argument.
 *
 * @param bitrate Bit rate (baud rate)
 * @param datalength Character (data) length
 * @param parity Parity
 * @param stopbit Stop bit length
 */
void SIOStartEx(int bitrate, int datalength, int parity, int stopbit);

/**
 * Shuts down SIO communication.
 */
void SIOStop(void);

/**
 * Read a single byte from the input buffer.
 * @return Data byte from input buffer
 */
unsigned char SIOReadByte(void);

/**
 * Send a single byte to the output buffer.
 * @param data Byte to send
 */
void SIOSendByte(unsigned char data);

/**
 * Check if any data is waiting in the input buffer.
 * Must be used when fetching data otherwise incorrect data could be read (usually 0x00).
 * @return Non-zero if there is data waiting in the input buffer, zero otherwise.
 */
int SIOCheckInBuffer(void);

/**
 * Check if port is ready to send data (previous operation finished).
 * Must be used when sending data as the output buffer is only 2 bytes long.
 * @return Non-zero if port is ready to send data, zero otherwise.
 */
int SIOCheckOutBuffer(void);



#endif
