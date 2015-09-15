/**
 * PSXSDK
 *
 * sys/stat.h
 */

#ifndef _SYS_STAT_H
#define _SYS_STAT_H

struct stat
{
	unsigned int st_size;
	unsigned int st_blksize;
	unsigned int st_blocks;
};

int stat(const char *path, struct stat *sb);

#endif
