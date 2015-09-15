#include "spasm.h"
#include "fcaseopen.h"

int rawArgc = 0;
char rawArgv[64][128];
char *curText;
char *curLine;
FILE *asmOut;
int line_number;

char *fileBuffer;
unsigned int fileBufferSize;

int open_case_sensitive = 0;

static void titlecard(void)
{
	printf("nv-spASM version 0.34.1 (c) 2014-2015 nextvolume\n");
}

static void usage(char *prog_name)
{
	titlecard();
	printf("Assembler for the Sony PlayStation\n");
	printf("\n");
	printf("Usage: %s [options] infile outfile\n", prog_name);
	printf("\n");
	printf("Options:   -b      create binary file instead of PS-X EXE\n");
	printf("           -N      do not pad executable\n");
	printf("           -S      be case sensitive on file paths\n");
	printf("\n");
}

FILE *spasm_fopen(const char *path, const char *mode)
{
	if(open_case_sensitive)
		return fopen(path, mode);
	
	return fcaseopen(path, mode);
}

int main(int argc, char *argv[])
{	
	int sz;
	int farg=1;
	int bfile=0;
	int no_newline=0;
	int no_pad=0;
	
	if(argc < 3)
	{
		usage(argv[0]);
		return EXIT_SUCCESS;
	}

	while(argv[farg][0] == '-')
	{
		if(strcmp(argv[farg], "-b") == 0)
			bfile = 1; // Generate binary file
		else if(strcmp(argv[farg], "-N") == 0)
			no_pad = 1;
		else if(strcmp(argv[farg], "-S") == 0)
			open_case_sensitive = 1;
		else
		{
			usage(argv[0]); 
			return EXIT_FAILURE;
		}
		
		farg++;
	}
			
	FILE *f = spasm_fopen(argv[farg], "rb");
	
	if(!f)
	{
		printf("Could not open file %s for reading! Exiting.\n", argv[farg]);
		return EXIT_FAILURE;
	}
	
	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	fileBufferSize = sz + 2; // two newline bytes + two NUL bytes
	fileBuffer = malloc(fileBufferSize);
	fread(fileBuffer, sizeof(char), sz, f);
	
	if(fileBuffer[sz-1] != '\n')
		no_newline = 1;
	
	fileBuffer[sz] = '\n';
	fileBuffer[sz+1] = '\0';
	
	//fileBuffer = process_includes(fileBuffer);
	
	fclose(f);

	asmOut = fopen(argv[farg+1], "wb");
	
	if(!asmOut)
	{
		printf("Could not open file %s for writing! Exiting.\n", argv[farg+1]);
		return EXIT_FAILURE;
	}
	
	startAddress = 0x80010000;

	titlecard();
	
	if(no_newline)
		printf("Warning: No newline found at end of file.\n");

	
	printf("\nPass 1\n");
	codegen_init();
	fileBuffer = spasm_parser(fileBuffer, -1);
	fileBuffer = spasm_parser(fileBuffer, 0);
	
	/*if(!org_found)
	{
		printf("Warning: no ORG directive found... defaulting to $80010000\n");
		startAddress = 0x80010000;
	}*/
		
	curPc = startAddress;
	
	printf("Pass 2\n");
	printf("Writing output file %s ...\n", argv[farg+1]);
	
	if(!bfile)
		OUTSEEK(0x800);	
	
	fileBuffer = spasm_parser(fileBuffer, 1);
	
	if(!bfile)
	{
		OUTSEEK(0x0);
		OUTSTRING("\"PS-X EXE\""); // PSX-EXE magic
		OUTSEEK(0x10);
		OUTWORD(startAddress); // Initial program counter
		OUTSEEK(0x18);
		OUTWORD(startAddress); // Text section start address
		OUTSEEK(0x30);
		OUTWORD(0x801FFFF0); // Initial stack pointer
	
		sz = OUTSIZE();
		
		
		if(!no_pad && (sz & 2047)) // Pad executable
		{
			sz |= 2047;
			sz++;
			OUTSEEK(sz-1);
			OUTBYTE(0);
		}
		
		OUTSEEK(0x1C);
		OUTWORD(sz - 0x800); // Output size of text section
	}
		
	printf("\nAssembly complete\n\n");
	
	fclose(asmOut);
	
	return EXIT_SUCCESS;
}
