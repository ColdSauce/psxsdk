/*
 * small libc functions for the PSXSDK
 *
 * In this file, C functions either not implemented by the BIOS
 * or very broken in the BIOS are implemented independently
 *
 * Some functions, such as printf, have both a BIOS implementation
 * and an implementation here. The implementation here is prefixed
 * with libc_ to not make confusion.
 */

#define __IN_LIBC

#include <psx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>

char onesec_buf[2048];
int errno;
int __stdio_direction = STDIO_DIRECTION_BIOS;
static unsigned int __sio_cr_mapped = 0;

FILE file_structs[256];

unsigned char file_state[256];

int libc_get_transtbl_fname(char *tofind, char *outstr, int outl);

enum
{
	FDEV_UNKNOWN,
	FDEV_CDROM,
	FDEV_MEMCARD
};

unsigned int fmode_to_desmode(const char *fmode)
{
	char rmode[16];
	int x, y;
	
	y = 0;

	for(x=0;x<15;x++)
	{
		if(fmode[x] == 0)
			break;
		else
		{
			if(fmode[x] != 'b' && fmode[x] != 'f')
				rmode[y++] = fmode[x];
		}
	}
	
	rmode[y] = 0;
	
	if(strcmp(rmode, "r") == 0)
	{
		dprintf("Open for reading.\n");
		return O_RDONLY;
	}
	else if(strcmp(rmode, "r+") == 0)
	{
		dprintf("Open for reading and writing.\n");
		return O_RDWR;
	}
	else if(strcmp(rmode, "w") == 0)
	{
		dprintf("Open for writing.\n");
		return O_WRONLY | O_CREAT | O_TRUNC;
	}
	else if(strcmp(rmode, "w+") == 0)
	{
		dprintf("Open for writing. Truncate to zero or create file.\n");
		return O_RDWR | O_CREAT | O_TRUNC;
	}
	else if(strcmp(rmode, "a") == 0)
	{
		dprintf("Append; open for writing. Create file if it doesn't exist.\n");
		return O_WRONLY | O_APPEND;
	}
	else if(strcmp(rmode, "a+") == 0)
	{
		dprintf("Append; open for reading and writing. Create file if it doesn't exist.\n");
		return O_RDWR | O_APPEND | O_CREAT;
	}
	else
	{
		return 0;
	}
}

FILE *fdopen(int fildes, const char *mode)
{
// Adjust for malloc
	int x;

// Find a free file structure	
	for(x = 0; x < 256; x++)
	{
		if(file_structs[x].used == 0)
		{
			file_structs[x].used = 1;
			break;
		}
	}

// If we found no free file structure, return NULL pointer
	
	if(x == 256)
		return NULL;
		

	file_structs[x].fildes = fildes;
	file_structs[x].pos = lseek(fildes, 0, SEEK_CUR);
	file_structs[x].mode = fmode_to_desmode(mode);

	return &file_structs[x];
}

FILE *fopen(char *path, const char *mode)
{
	int fd;
	FILE *f;
	char *s = NULL;
	
	if(strncmp(path, "cdromL:", 7) == 0)
	{
		//printf("strncmp=%d\n", strncmp(path, "cdromL:", 7) );
		s = malloc(1024);

		if(libc_get_transtbl_fname(path+7, s, 1024) == 0)
			return NULL;

		fd = open(s, fmode_to_desmode(mode));
	}
	else
		fd = open(path, fmode_to_desmode(mode));

	if(fd == -1)
	{
		if(s!=NULL)free(s);
		return NULL;
	}
		
	f = fdopen(fd, mode);
	
	if(f == NULL)
	{
		if(s!=NULL)free(s);
		return NULL;
	}
	
	f->dev = FDEV_UNKNOWN;
	
	if(strncmp(path, "cdrom", 5) == 0 || strncmp(path, "cdromL", 6) == 0)
		f->dev = FDEV_CDROM;
	else if(strncmp(path, "bu", 2) == 0)
		f->dev = FDEV_MEMCARD;
	
	if(s!=NULL)
	{
		f->size = get_real_file_size(s);
		free(s);
	}
	else
		f->size = get_real_file_size(path);
		
	return f;
}

int fclose(FILE *stream)
{
	stream->used = 0;
	close(stream->fildes);
	return 0;
}

/*
 * fread doesn't require reads to be carried in block unit
 * Notice that however seeks on the CD drive will be very slow - so avoid using non block units
 *
 * This is done to make programming and porting easier 
 */

int fread(void *ptr, int size, int nmemb, FILE *f)
{
	int rsize = size * nmemb;
	int csize = rsize;
	int max;
	int nsect = (f->pos + rsize) >> 11; 
	nsect -= f->pos >> 11;
	nsect++;
	
	//printf("f->dev = %d, f->pos = %d, rsize = %d\n", f->dev, f->pos, rsize);

	if(f->dev == FDEV_CDROM)
	{
		// First sector
		lseek(f->fildes, f->pos & (~0x7ff), SEEK_SET);
		read(f->fildes, onesec_buf, 2048);
		
		max = 2048 - (f->pos & 2047);
		
		//printf("ptr(FIRST) = %d, %x\n", ptr, ptr);
		printf("rsize = %d\n", rsize);
		
		memcpy(ptr, onesec_buf + (f->pos & 2047), (rsize > max) ? max : rsize);
		
		// Middle sector
		ptr += max;
		
		//printf("ptr(MIDDLEsex) = %d, %x\n", ptr, ptr);
		nsect--;
		csize -= max;
		
		if(nsect > 1)
		{
			//lseek(f->fildes, (f->pos & (~0x7ff)) + 2048, SEEK_SET);

//#warning "Check correctness of this calculation."
		
			/*if(rsize & 2047)
				sect_num = (rsize|2047)+1;
			else
				sect_num = rsize;
				
			sect_num -= 4096;*/
			
			//printf("read_middle=%d, sect_num = %d\n", read(f->fildes, ptr, sect_num), sect_num);
			
			read(f->fildes, ptr, (nsect - 1) * 2048);
			
			ptr += (nsect - 1) * 2048;
			csize -= (nsect - 1) * 2048;
			nsect = 1;
		}
		
		//printf("ptr(LAST) = %d, %x\n", ptr, ptr);
		
		if(nsect == 1)
		{
			// Last sector
			read(f->fildes, onesec_buf, 2048);
		
			memcpy(ptr, onesec_buf, csize);
		}
	}	

	f->pos+= rsize;	
	return rsize;
}

int fgetc(FILE *f)
{
	unsigned char c;
		
	if(f->pos >= f->size)
		return EOF;
	
	fread(&c, sizeof(char), 1, f);
		
	return (int)c;
}
	
int ftell(FILE *f)
{
	return f->pos;
}

int fseek(FILE *f, int offset, int whence)
{
	switch(whence)
	{
		case SEEK_SET:
			f->pos = offset;
		break;
		case SEEK_CUR:
			f->pos+= offset;
		break;
		case SEEK_END:
			f->pos = f->size + offset;
		break;
		default:
			f->pos = whence + offset;
		break;
	}
	
	return 0;
}

int toupper(int c)
{
	if(c >= 'a' && c <= 'z')
		return (c-'a')+'A';

	return c;
}

int tolower(int c)
{
	if(c >= 'A' && c <= 'Z')
		return (c-'A')+'a';

	return c;
}

int libc_get_transtbl_fname(char *tofind, char *outstr, int outl)
{
	FILE *f;
	int s;
	int x;
	int type;
	int y;
	int l = strlen(tofind);
	int filename_found = 0;
	int exit_loop = 0;
	int otfp = 0;
	int tfp = 0;
	char transtbl[0x4000];
	char orgname[16];
	char newname[256];
	char rootpath[256];

	bzero(transtbl, 0x4000);
	strcpy(rootpath, "cdrom:\\");

	f = fopen("cdrom:\\TRANS.TBL;1", "rb");

	if(f == NULL)
		return 0;

	fseek(f, 0, SEEK_END);
	s = ftell(f);
	fseek(f, 0, SEEK_SET);
	fread(transtbl, 1, s, f);
	fclose(f);

	outstr[0] = 0;

	x = 0;

	exit_loop = 0;
	filename_found = 0;

	for(tfp = 0; tofind[tfp] == '\\' || tofind[tfp] == '/'; tfp++);

	otfp = tfp;

	for(y = otfp; y < l; y++)
	{
		if(tofind[y] == '\0' || tofind[y] == '\\' || tofind[y] == '/')
			break;
	}

	tfp = y;

	while((x < s) && !exit_loop)
	{
		while(transtbl[x] == ' ' || transtbl[x] == '\t' || transtbl[x] == '\n' || transtbl[x] == '\r')
			x++;

		if(transtbl[x] == 'F')
			type = 0;
		else if(transtbl[x] == 'D')
			type = 1;

		x++;

		while(transtbl[x] == ' ' || transtbl[x] == '\t' || transtbl[x] == '\n' || transtbl[x] == '\r')
			x++;

		y = 0;

		while(!(transtbl[x] == ' ' || transtbl[x] == '\t' || transtbl[x] == '\n' || transtbl[x] == '\r'
		|| transtbl[x] == 0))
				orgname[y++] = transtbl[x++];

		orgname[y] = 0;
		//printf("orgname = %s\n", orgname);

		while(transtbl[x] == ' ' || transtbl[x] == '\t' || transtbl[x] == '\n' || transtbl[x] == '\r')
			x++;

		y = 0;

		while(!(transtbl[x] == '\n' || transtbl[x] == '\r' || transtbl[x] == 0))
			newname[y++] = transtbl[x++];

		newname[y] = 0;

		while(transtbl[x] == ' ' || transtbl[x] == '\t' || transtbl[x] == '\n' || transtbl[x] == '\r')
			x++;

		//printf("newname = %s\n", newname);

		if(strncasecmp(&tofind[otfp],  newname, tfp-otfp) == 0)
		{
			if(type == 0)
			{
				dprintf("Filename found: %s -> %s%s\n", tofind, rootpath, orgname);
				filename_found = 1;
				exit_loop = 1;

				strncpy(outstr, rootpath, outl);
				strncat(outstr, orgname, outl-strlen(rootpath));
			}
			else
			{
				//printf("Found directory! %s\n", newname);

				//printf("tfp = %d\n", tfp);

				if(tfp == l || tofind[l-1] == '/'
					|| tofind[l-1] == '\\')
				{
					dprintf("File not found. A directory was specified.\n");
					exit_loop = 1;
					continue;
				}

				//tfp++;
				for(; tofind[tfp] == '\\' || tofind[tfp] == '/'; tfp++);

				otfp = tfp;
	
				for(y = otfp; y < l; y++)
				{
					if(tofind[y] == '\0' || tofind[y] == '\\' || tofind[y] == '/')
						break;
				}

				tfp = y;
	
				strcat(rootpath, orgname);
				strcat(rootpath, "\\");

				y = strlen(rootpath);
				strcat(rootpath, "TRANS.TBL;1");

				bzero(transtbl, 0x4000);

				f = fopen(rootpath, "rb");

				if(f == NULL)
				{
					dprintf("Couldn't find %s\n", rootpath);
					exit_loop = 1;
					continue;
				}

				rootpath[y] = 0;

				fseek(f, 0, SEEK_END);
				s = ftell(f);
				fseek(f, 0, SEEK_SET);
				fread(transtbl, 1, s, f);
				fclose(f);
				
				x = 0;
			}
		}
	}

	return filename_found;
}

int isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}

int islower(int c)
{
	return (c >= 'a' && c <= 'z');
}

int isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

int isxdigit(int c)
{
	return ((c >= '0' && c <= '9') || (c >= 'A' && c<='F') || (c >= 'a' && c<='f'));
}

int isalpha(int c)
{
	return ((c>='a' && c<='z') || (c>='A' && c<='Z'));
}

int isalnum(int c)
{
	return ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'));
}

int isspace(int c)
{
	return ((c == ' ') || (c == '\f') || (c == '\n') || (c == '\r') || (c == '\t') || (c == '\v'));
}

int isprint(int c)
{
	return (c >= 0x20) && (c <= 0x7E);
}

int isgraph(int c)
{
	return (c > 0x20) && (c <= 0x7E);
}

int iscntrl(int c)
{
	return (c < 0x20);
}

int isblank(int c)
{
	return ((c == ' ') || (c == '\t'));
}

void redirect_stdio_to_sio(void)
{
	__stdio_direction = STDIO_DIRECTION_SIO;
}

void sio_stdio_mapcr(unsigned int setting)
{
	__sio_cr_mapped = setting;
}

int sio_putchar(int c)
{
	if(c == '\n' && __sio_cr_mapped)
		sio_putchar('\r');
	
	while(!SIOCheckOutBuffer());
	
	SIOSendByte(c);
	
	return c;
}

int sio_puts(const char *str)
{
	while(*str)
		sio_putchar(*(str++));
	
	sio_putchar('\n');
	
	return 1;
}

extern int bios_putchar(int c);
extern int bios_puts(const char *str);

int putchar(int c)
{
	switch(__stdio_direction)
	{
		case STDIO_DIRECTION_BIOS:
			return bios_putchar(c);
		break;
		case STDIO_DIRECTION_SIO:
			return sio_putchar(c);
		break;
	}
	
	return -1;
}

int puts(const char *str)
{
	switch(__stdio_direction)
	{
		case STDIO_DIRECTION_BIOS:
			return bios_puts(str);
		break;
		case STDIO_DIRECTION_SIO:
			return sio_puts(str);
		break;
	}
	
	return -1;
}
