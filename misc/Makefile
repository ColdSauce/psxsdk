include ../Makefile.cfg

all: 
	sh genscripts.sh $(TOOLCHAIN_PREFIX)

clean:
	rm -f playstation.x psx-gcc psx-g++

distclean: clean

install: all
	mkdir -p $(TOOLCHAIN_PREFIX)/bin
	mkdir -p $(TOOLCHAIN_PREFIX)/mipsel-unknown-elf/lib/ldscripts
	cp -v psx-gcc $(TOOLCHAIN_PREFIX)/bin
	cp -v psx-g++ $(TOOLCHAIN_PREFIX)/bin
	cp -v playstation.x $(TOOLCHAIN_PREFIX)/mipsel-unknown-elf/lib/ldscripts
