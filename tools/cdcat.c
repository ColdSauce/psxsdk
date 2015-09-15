/*
 * cdcat: get and replace files inside an ISO-9660 CDROM image
 * Based on cdcat by Robert Nordier
 *
 * Copyright (c) 2011 Giuseppe Gatta
 *
 * Copyright (c) 2007 Robert Nordier.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted subject to the following conditions:
 *
 *1. Existing copyright notices in source and other files must be
 *  retained.
 *
 *2. Redistributions in whatever form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in
 *  the documentation or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define CDCAT_VERSION	"0.5"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum
{
	cdrom_mode_1,
	cdrom_mode_1_raw,
	cdrom_mode_2,
};

enum
{
	cdcat_oper_read,
	cdcat_oper_write,
	cdcat_oper_showoffset
};

int cdcat_cdrom_mode = cdrom_mode_1;
int cdcat_oper = cdcat_oper_read;

#define SECSIZ 2048
#define NAMLEN 255

#define sw(x,y) ((x)<<8|(y))
#define cv4(x) ((*(unsigned char*)(x)) | ((*(unsigned char*)(x+1))<<8) | \
	       ((*(unsigned char*)(x+2))<<16) | ((*(unsigned char*)(x+3))<<24))

/* ISO 9660 Primary Volume Descriptor */
static char cdmagic[] = {1, 'C', 'D', '0', '0', '1', 1, 0};

struct cddir {
	unsigned char len_dr;	/* length of directory record */
	unsigned char len_ear;	/* extended attribute record length */
	unsigned char ext[8];	/* location of extent */
	unsigned char size[8];	/* data length */
	unsigned char time[7];	/* recording date and time */
	unsigned char flags;	/* file flags */
	unsigned char fus;	/* file unit size */
	unsigned char gap;	/* interleave gap size */
	unsigned char vsn[4];	/* volume sequence number */
	unsigned char len_fi;	/* length of file identifier */
	unsigned char fi[1];	/* file identifier ... */
};

struct dir {
	unsigned int ext;		/* starting block number */
	unsigned int size;		/* file size */
	int type;		/* file type ('d' or '-') */
	char name[NAMLEN + 1];	/* file name */
};

static char *fn;		/* special file name */
static int fd;			/* special file descriptor */

int cdcat(char *);
void loaddir(struct cddir *, struct dir *);
void susp(unsigned char *, int, struct dir *);
int readblk(void *, unsigned int);
void writeblk(void *, unsigned int);
void error(char *);

void cdcat_print_usage()
{
	fprintf(stderr, "usage: cdcat <options> iso-image [path]\n");
	fprintf(stderr, "\n"
			      "cdcat can be used to explore an ISO9660 filesystem image\n"
			      "If path is not specified the root directory is listed, if it's a directory "
			      "the directory is listed or if it's a file the file is printed to standard output\n"
			      "The Rock Ridge extensions to the ISO9660 filesystem standard are supported\n"
			      "Original program (c) 2007 Robert Nordier.\n"
			      "\n"
			      "Options:\n"
			      "-help           -         This screen\n"
			      "-mode1          -         The image is raw and has Mode 1 sectors\n"
			      "-mode2          -         The image is raw and has Mode 2 sectors (like PlayStation dumps)\n"
			      "-replace        -         If [path] is specified, the data of the file is\n"
			      "                          replaced with input from standard input\n"
			      "-showoffset     -         Show file offset in image\n"
			      "-version        -         Display version\n\n");
}

int main(int argc, char **argv)
{
	char *path;
	int e;
	int nargc;
	int x;

	for(x = 1; x < argc; x++)
	{
		if(strcmp(argv[x], "--") == 0 || argv[x][0] != '-')
			break;

		if(strcmp(argv[x], "-mode2") == 0 || strcmp(argv[x], "--mode2") == 0)
			cdcat_cdrom_mode = cdrom_mode_2;
		else if(strcmp(argv[x], "-mode1") == 0 || strcmp(argv[x], "--mode1") == 0)
			cdcat_cdrom_mode = cdrom_mode_1_raw;
		else if(strcmp(argv[x], "--help") == 0 || strcmp(argv[x], "-help") == 0)
		{
			cdcat_print_usage();
			return 0;
		}
		else if(strcmp(argv[x], "--version") == 0 || strcmp(argv[x], "-version") == 0)
		{
			printf("cdcat version "CDCAT_VERSION"\n");
			return 0;
		}
		else if(strcmp(argv[x], "--replace") == 0 || strcmp(argv[x], "-replace") == 0)
			cdcat_oper = cdcat_oper_write;
		else if(strcmp(argv[x], "--showoffset") == 0 || strcmp(argv[x], "-showoffset") == 0)
			cdcat_oper = cdcat_oper_showoffset;
		else
		{
			printf("Invalid option %s! Aborting.\n", argv[x]);
			return -1;
		}
	}

	nargc = argc-(x-1);

	if (nargc != 2 && nargc != 3) {
		cdcat_print_usage();
		exit(2);
	}

	fn = argv[x];

	if ((fd = open(argv[x], cdcat_oper==cdcat_oper_read?O_RDONLY:O_RDWR)) == -1)
		error("cannot open");
	path = argv[x+1] ? argv[x+1] : "";
		
	if ((e = cdcat(path)) != 0)
		fprintf(stderr, "cdcat: %s: Not found\n", path);
	return e;
}

int cdcat(char *path)
{
	unsigned char buf[SECSIZ];
	char name[NAMLEN + 1];
	struct cddir *dp, *tp;
	struct dir xd;
	char *p, *q;
	unsigned ext, size, bx, bn, x, i;
	int type, n;

	/* 
	 * find primary volume descriptor
	 * and thence root directory
	 */
	bx = 64;
	for (bn = 16; bn < bx; bn++) {
		readblk(buf, bn);
		if (strcmp((char *)buf, cdmagic) == 0)
			break;
	}
	if (bn == bx)
		error("Invalid argument");
	dp = (struct cddir *)&buf[156];
	loaddir(dp, &xd);

	/*
	 * lookup, list, print ...
	 */
	for (p = path; dp; p = q) {
		while (*p == '/')
			p++;
		for (q = p; *q && *q != '/'; q++);
		if ((n = q - p)) {
			if (n > NAMLEN)
				n = NAMLEN;
			memcpy(name, p, n);
			name[n] = 0;
		}
		ext = xd.ext;
		size = xd.size;
		type = xd.type;
		dp = NULL;
		bx = ext + (size + (SECSIZ - 1)) / SECSIZ;
		for (bn = ext; !dp && bn < bx; bn++) {
			if (type == 'd')
			{
				readblk(buf, bn);

				for (i = 0; !dp && buf[i]; i += buf[i]) {
					tp = (struct cddir *)(buf + i);
					loaddir(tp, &xd);
					if (n == 0)
						printf("%10u %c %s\n",
						    xd.size, xd.type, xd.name);
					else if (strcmp(name, xd.name) == 0)
						dp = tp;
				}
			}
			else {
				if(cdcat_oper == cdcat_oper_read)
				{
					readblk(buf, bn);
					x = size < SECSIZ ? size : SECSIZ;
					for (i = 0; i < x; i++)
						putchar(buf[i]);
					size -= x;
				}else if(cdcat_oper == cdcat_oper_write)
				{
					x = size < SECSIZ ? size : SECSIZ;
					for (i = 0; i < x; i++)
						buf[i] = getchar();

					writeblk(buf, bn);

					size -= x;
				}else if(cdcat_oper == cdcat_oper_showoffset)
				{
					printf("%d\n", readblk(buf, bn));
					goto cdcat_end;
				}
			}
		}
	}
cdcat_end:
	return n != 0;
}

/*
 * Gather together the directory information that interests us.
 * Any and all of this may be altered by a suitable SUSP field.
 */
void loaddir(struct cddir *dp, struct dir *xp)
{
	int c;

	xp->ext = cv4(dp->ext);
	xp->size = cv4(dp->size);
	xp->type = dp->flags & 2 ? 'd' : '-';
	xp->name[0] = 0;
	if (dp->fi[0] != 0) {
		c = dp->len_fi | 1;
		susp(dp->fi + c, dp->len_dr - 33 - c, xp);
	}
	if (xp->name[0] == 0)
	{
		if (dp->fi[0] == 0 || dp->fi[0] == 1)
			strcpy(xp->name, dp->fi[0] == 0 ? "." : "..");
		else
		{
			memcpy(xp->name, dp->fi, dp->len_fi);
			xp->name[dp->len_fi] = 0;
		}
	}
}

/*
 * SUSP/RRIP support: allowing UNIX-style file names and directories
 * nested more than eight deep (among other things).
 */
void susp(unsigned char *sp, int n, struct dir *xp)
{
	unsigned char buf[SECSIZ];
	unsigned char *p;
	int i, j;

	for (p = sp; p < sp + n && *p;) {
		if (p[3] != 1)
			return;
		switch (sw(p[0], p[1])) {
		/* continuation area */
		case sw('C', 'E'):
			readblk(buf, cv4(&p[4]));
			sp = buf + cv4(&p[12]);
			n = cv4(&p[20]);
			p = sp;
			continue;
		/* child link */
		case sw('C', 'L'):
			xp->ext = cv4(&p[4]);
			xp->size = SECSIZ;
			xp->type = 'd';
			break;
		/* alternate name */
		case sw('N', 'M'):
			for (j = 0; xp->name[j]; j++);
			for (i = 5; i < p[2]; i++)
				xp->name[j++] = p[i];
			xp->name[j] = 0;
			break;
		}
		p += p[2];
	}
}

int readblk(void *buf, unsigned int blkno)
{
	int r;
	
	switch(cdcat_cdrom_mode)
	{
		case cdrom_mode_1:
			r = lseek(fd, blkno * SECSIZ, 0);
		break;
		case cdrom_mode_1_raw:
			r = lseek(fd, (blkno * 2352) + 16, 0);
		break;
		case cdrom_mode_2:
			r = lseek(fd, (blkno * 2352) + 24, 0);
		break;
	}

	if (read(fd, buf, SECSIZ) != SECSIZ)
			error("read error");
	
	return r;
}

void writeblk(void *buf, unsigned int blkno)
{
	switch(cdcat_cdrom_mode)
	{
		case cdrom_mode_1:
			lseek(fd, blkno * SECSIZ, 0);
		break;
		case cdrom_mode_1_raw:
			lseek(fd, (blkno * 2352) + 16, 0);
		break;
		case cdrom_mode_2:
			lseek(fd, (blkno * 2352) + 24, 0);
		break;
	}

	if (write(fd, buf, SECSIZ) != SECSIZ)
		error("write error");
}

void error(char *msg)
{
	fprintf(stderr, "cdcat: %s: %s\n", fn, msg);
	exit(2);
}
