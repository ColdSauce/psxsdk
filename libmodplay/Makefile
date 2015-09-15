# Makefile for libmodplay

include ../Makefile.cfg

CFLAGS += -I../libadpcm

all: libmodplay.a libmodplay_nopsx.a

modplay.o: modplay.c
	$(CC) $(CFLAGS) -c modplay.c
	
mod.o: mod.c
	$(CC) $(CFLAGS) -c mod.c

libmodplay.a: modplay.o mod.o
	rm -f libmodplay.a
	$(AR) r libmodplay.a modplay.o mod.o
	$(RANLIB) libmodplay.a

modplay_nopsx.o: modplay.c
	$(HOST_CC) $(HOST_CFLAGS) -DNO_PSX_LIB -c modplay.c -o modplay_nopsx.o

mod_nopsx.o: mod.c
	$(HOST_CC) $(HOST_CFLAGS) -DNO_PSX_LIB -c mod.c -o mod_nopsx.o

c669_nopsx.o: c669.c
	$(HOST_CC) $(HOST_CFLAGS) -DNO_PSX_LIB -c c669.c -o c669_nopsx.o

mtm_nopsx.o: mtm.c
	$(HOST_CC) $(HOST_CFLAGS) -DNO_PSX_LIB -c mtm.c -o mtm_nopsx.o
	
xm_nopsx.o: xm.c
	$(HOST_CC) $(HOST_CFLAGS) -DNO_PSX_LIB -c xm.c -o xm_nopsx.o

it_nopsx.o: it.c
	$(HOST_CC) $(HOST_CFLAGS) -DNO_PSX_LIB -c it.c -o it_nopsx.o

libmodplay_nopsx.a: modplay_nopsx.o mod_nopsx.o
	rm -f libmodplay_nopsx.a
	$(HOST_AR) r libmodplay_nopsx.a modplay_nopsx.o mod_nopsx.o
	$(HOST_RANLIB) libmodplay_nopsx.a

install: all
	cp libmodplay.a $(TOOLCHAIN_PREFIX)/lib
	cp modplay.h $(TOOLCHAIN_PREFIX)/include

clean:
	rm -f *.o *.a

distclean: clean
