/*
 * unistd.h
 *
 * PSXSDK
 */

#ifndef _UNISTD_H
#define _UNISTD_H

#include <types.h>

int lseek(int fd, int off, int whence);
int read(int fd, void *buf, int nbytes);
int write(int fd, void *buf, int nbytes);
int close(int fd);
int cd(char *dirname);

void swab(const void *src, void *dst, ssize_t len);

#define chdir(dirname)	cd(dirname)

#endif
