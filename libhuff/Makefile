# Makefile for libhuff

include ../Makefile.cfg

#CC = mipsel-unknown-elf-gcc
#CFLAGS = -I../libpsx/include/ -G0 -O0 -fno-builtin -mno-gpopt -nostdlib -msoft-float -g -Wall

all: libhuff.a

huff.o: huff.c
	$(CC) $(CFLAGS) -c huff.c

libhuff.a: huff.o
	rm -f libhuff.a
	$(AR) r libhuff.a huff.o
	$(RANLIB) libhuff.a	

install: all
	cp libhuff.a $(TOOLCHAIN_PREFIX)/lib
	cp huff.h $(TOOLCHAIN_PREFIX)/include

clean:
	rm -f *.o *.a