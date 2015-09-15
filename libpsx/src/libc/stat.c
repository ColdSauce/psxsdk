#include <psx.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int stat(const char *path, struct stat *sb)
{
	struct DIRENTRY dir_e;
	
	if(firstfile((char*)path, &dir_e) == NULL)
		return -1;
	
	sb->st_size = dir_e.size;
	
	if(strncmp(path, "cdrom:", 6) == 0)
		sb->st_blksize = 2048;
	else if(strncmp(path, "bu00:", 5) == 0 ||
		    strncmp(path, "bu10:", 5) == 0)
		sb->st_blksize = 128;
	else
// not a real blocksize, will be there just as a placeholder
		sb->st_blksize = 1024; 
	
	sb->st_blocks = 
		sb->st_size / sb->st_blksize; 
	
	if(sb->st_size % sb->st_blksize)
		sb->st_blocks++;
	
	return 0;
}
