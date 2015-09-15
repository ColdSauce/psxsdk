/*
 * SIO communication library for PSXSDK.
 * Originally written by Shendo.
 * Thanks to Martin Korth of the NO$PSX for documentation.
 * 
 * This library is accessing SIO registers directly, no BIOS routines are used.
 */

#include <psx.h>
#include <psxsio.h>
#include <stdio.h>

void SIOStart(int bitrate)
{
	/*Set to 8N1 mode with desired bitrate*/
	SIOStartEx(bitrate, SIO_DATA_LEN_8, SIO_PARITY_NONE, SIO_STOP_BIT_1);
}

void SIOStartEx(int bitrate, int datalength, int parity, int stopbit)
{
	/*Set SIO_MODE register, bitrate reload factor set to MUL16 by default*/
	SIO_MODE = SIO_REL_MUL16 | (datalength << 2) | (parity << 4) | (stopbit << 6);

	/*Reset SIO_CTRL register.*/
	SIO_CTRL = 0;

	/*Set TX and RT to enabled, no handshaking signals.*/
	SIO_CTRL = 1 | (1 << 2);

	/*Calculate bitrate reload value based on the given bitrate
	 * Reload = SystemClock (33 Mhz) / (Factor (MULI16) * bitrate)*/
	SIO_BPSV = 0x204CC00 / (16 * bitrate);
}

void SIOStop()
{
	/*Set all SIO related registers to zero*/
	SIO_MODE = 0;
	SIO_CTRL = 0;
	SIO_BPSV = 0;
}

unsigned char SIOReadByte()
{
	return (unsigned char)SIO_TX_RX;
}

void SIOSendByte(unsigned char data)
{
	SIO_TX_RX = data;
}

int SIOCheckInBuffer()
{
	/*Return status of RX FIFO*/
	return (SIO_STAT & 0x2)>0;
}

int SIOCheckOutBuffer()
{
	/*Return status of TX Ready flag*/
	return (SIO_STAT & 0x4)>0;
}
	