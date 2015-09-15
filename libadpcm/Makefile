# Makefile for libadpcm

include ../Makefile.cfg

AR = mipsel-unknown-elf-ar
RANLIB = mipsel-unknown-elf-ranlib
CFLAGS += -O3 -I../libm

all: libadpcm.a

adpcm.o: adpcm.c
	$(CC) $(CFLAGS) -c adpcm.c

libadpcm.a: adpcm.o
	rm -f libadpcm.a
	$(AR) r libadpcm.a adpcm.o
	$(RANLIB) libadpcm.a	

install: all
	cp libadpcm.a $(TOOLCHAIN_PREFIX)/lib
	cp adpcm.h $(TOOLCHAIN_PREFIX)/include

clean:
	rm -f *.o *.a

distclean: clean
