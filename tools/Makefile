# Makefile for the PSXSDK tools

include ../Makefile.cfg

TOOL_LIST = bmp2tim$(EXE_SUFFIX) \
		   cdcat$(EXE_SUFFIX) \
		   elf2exe$(EXE_SUFFIX) \
		   getpsxiso$(EXE_SUFFIX) \
		   mkpsxiso$(EXE_SUFFIX) \
		   vag2wav$(EXE_SUFFIX) \
		   wav2vag$(EXE_SUFFIX) \
		   exefixup$(EXE_SUFFIX) \
		   systemcnf$(EXE_SUFFIX) \
		   bin2c$(EXE_SUFFIX) \
		   huff$(EXE_SUFFIX) \
		   mod4psx$(EXE_SUFFIX) \
		   tim2bmp$(EXE_SUFFIX) \
		   lictool$(EXE_SUFFIX) \
		   psfex$(EXE_SUFFIX)

all: $(TOOL_LIST)
	$(MAKE_COMMAND) -C spasm

bmp2tim$(EXE_SUFFIX): bmp2tim.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ bmp2tim.c $(HOST_LDFLAGS)

cdcat$(EXE_SUFFIX): cdcat.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ cdcat.c $(HOST_LDFLAGS)

elf2exe$(EXE_SUFFIX): elf2exe.c
	$(HOST_CC) $(HOST_CFLAGS) -DOBJCOPY_PATH=\"$(OBJCOPY)\" -o $@ elf2exe.c $(HOST_LDFLAGS)

getpsxiso$(EXE_SUFFIX): getpsxiso.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ getpsxiso.c $(HOST_LDFLAGS)

mkpsxiso$(EXE_SUFFIX): mkpsxiso.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ mkpsxiso.c $(HOST_LDFLAGS)
	
vag2wav$(EXE_SUFFIX): vag2wav.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ vag2wav.c $(HOST_LDFLAGS)
	
wav2vag$(EXE_SUFFIX): wav2vag.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ wav2vag.c $(HOST_LDFLAGS)

exefixup$(EXE_SUFFIX): exefixup.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ exefixup.c $(HOST_LDFLAGS)
	
systemcnf$(EXE_SUFFIX): systemcnf.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ systemcnf.c $(HOST_LDFLAGS)

bin2c$(EXE_SUFFIX): bin2c.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ bin2c.c $(HOST_LDFLAGS)

huff$(EXE_SUFFIX): huff.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ huff.c $(HOST_LDFLAGS)

mod4psx$(EXE_SUFFIX): mod4psx.c adpcm.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ mod4psx.c adpcm.c ../libmodplay/libmodplay_nopsx.a -lm -DNO_PSX_LIB

tim2bmp$(EXE_SUFFIX): tim2bmp.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ tim2bmp.c -lz $(HOST_LDFLAGS)
	
lictool$(EXE_SUFFIX): lictool.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ lictool.c $(HOST_LDFLAGS)

psfex$(EXE_SUFFIX): psfex.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ psfex.c -lz $(HOST_LDFLAGS)

clean:
	rm -f $(TOOL_LIST)
	$(MAKE_COMMAND) -C spasm clean

distclean: clean

install:
	cp -rv $(TOOL_LIST) $(TOOLCHAIN_PREFIX)/bin
	$(MAKE_COMMAND) -C spasm install
