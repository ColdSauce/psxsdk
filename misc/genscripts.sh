#!/bin/sh

# This shell script will generate the playstation.x linker script, and the psx-gcc and psx-elf2x shell scripts.

# You have to pass the PREFIX of the toolchain as the first argument of this shell script

echo "/* 
 * Linker script to generate an ELF file
 * that has to be converted to PS-X EXE.
 */

TARGET(\"elf32-littlemips\")
OUTPUT_ARCH(\"mips\")

ENTRY(\"_start\")

SEARCH_DIR(\"$1/lib\")
STARTUP(start.o)
INPUT(-lpsx -lgcc)

SECTIONS
{
	. = 0x80010000;

	__text_start = .;
	.text ALIGN(4) : { *(.text*) }
	__text_end = .;

	__rodata_start = .;
	.rodata ALIGN(4) : { *(.rodata) }
	__rodata_end = .;

	__data_start = .;
	.data ALIGN(4) : { *(.data) }
	__data_end = .;
	
	__ctor_list = .;
	.ctors ALIGN(4) : { *(.ctors) }
	__ctor_end = .;
	
	__dtor_list = .;
	.dtors ALIGN(4) : { *(.dtors) }
	__dtor_end = .;

	__bss_start = .;
	.bss  ALIGN(4) : { *(.bss) }
	__bss_end = .;

	__scratchpad = 0x1f800000;
}
" > playstation.x

echo "#!/bin/sh
mipsel-unknown-elf-gcc -D__PSXSDK__ -fno-strict-overflow -fsigned-char -msoft-float -mno-gpopt -fno-builtin -G0 -I$1/include -T $1/mipsel-unknown-elf/lib/ldscripts/playstation.x \$*"> psx-gcc
chmod +x psx-gcc
echo "#!/bin/sh
mipsel-unknown-elf-g++ -D__PSXSDK__ -fno-strict-overflow -fsigned-char -msoft-float -mno-gpopt -fno-builtin -G0 -I$1/include -T $1/mipsel-unknown-elf/lib/ldscripts/playstation.x -fno-rtti -fno-exceptions -fno-threadsafe-statics -fno-use-cxa-atexit \$*" > psx-g++
chmod +x psx-g++
