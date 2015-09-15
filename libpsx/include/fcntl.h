/*
 * fcntl.h
 *
 * File control
 *
 * PSXSDK
 */

#ifndef _FCNTL_H
#define _FCNTL_H

#define O_RDONLY		1
#define O_WRONLY		2
#define O_NONBLOCK	4
/** Read locked, not shared. */
#define O_RDLOCK		16
/** Write locked, not shared. */
#define O_WRLOCK		32
#define O_RDWR		(O_RDONLY | O_WRONLY)
#define O_APPEND		256
#define O_CREAT		512
#define O_TRUNC		1024
/** Obviously, O_EXCL has no effect in the PSXSDK. */
#define O_EXCL		0

/** The following are PlayStation BIOS extensions. */

/** Set to scanning type. Real purpose unknown. */
#define O_SCAN		4096
/** Setup for remote command entry. Real purpose unknown. */
#define O_RCOM		8192
/**  No buffering and console interrupt */
#define O_NOBUF		16384
/** Asynchronous I/O mode */
#define O_NOWAIT		32768
/** Asynchronous I/O mode, alias */
#define O_ASYNC		O_NOWAIT

/**
 * These are standard C library file I/O functions provided by the PSX BIOS.
 * Filenames have to be specified in this way:
 * [device]:[filename]
 *
 * Where device specifies the device the file is on:
 * "tty:"		Console
 * "cdrom:"		CD-ROM
 * "buXX:"		Memory cards
 *
 * When using cdrom: as device, append file version (;1) to filename
 * Example: cdrom:README.TXT;1
 *
 * Subdirectory paths have to be specified with backslashes (\),
 * like MS-DOS. Read and write operations can be carried only in blocks. 
 * Blocks are 2048 bytes for the CD-ROM device, and 128 bytes for memory cards.
 */

// The third argument (file mode) makes no sense in PSXSDK,
// and will be ignored.

#define open(filename, flags, ...) \
	open(filename, flags)

/** In previous versions, the second argument for open()
     was named `mode'. That was incorrect; now it is correctly named `flags'.
*/

int open(char *filename, int flags);
int read(int d, void *buf, int nbytes);
int close(int d);
int lseek(int fildes, int offset, int whence);

#endif
