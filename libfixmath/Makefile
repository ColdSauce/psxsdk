include ../Makefile.cfg

#Project settings
PROJECT = libfixmath
LIB =
SRC = .
INC =

#Compiler settings
AR = mipsel-unknown-elf-ar
CPP_FLAGS = $(CFLAGS) -O2 $(INC) -c
CC_FLAGS  = $(CFLAGS) -O2 $(INC) -c
AS_FLAGS  = $(CC_FLAGS) -D_ASSEMBLER_
LD_FLAGS = -Wall

# Find all source files
SRC_CPP = $(foreach dir, $(SRC), $(wildcard $(dir)/*.cpp))
SRC_C   = $(foreach dir, $(SRC), $(wildcard $(dir)/*.c))
SRC_S   = $(foreach dir, $(SRC), $(wildcard $(dir)/*.S))
OBJ_CPP = $(patsubst %.cpp, %.o, $(SRC_CPP))
OBJ_C   = $(patsubst %.c, %.o, $(SRC_C))
OBJ_S   = $(patsubst %.S, %.o, $(SRC_S))
OBJ     = $(OBJ_CPP) $(OBJ_C) $(OBJ_S)

# Compile rules.
.PHONY : all
all: $(PROJECT).a

$(PROJECT).a: $(OBJ)
	$(AR) rcs $(PROJECT).a $(OBJ)

$(OBJ_CPP) : %.o : %.cpp
	$(CPP) $(CPP_FLAGS) -o $@ $<

$(OBJ_C) : %.o : %.c
	$(CC) $(CC_FLAGS) -Ifixmath -o $@ $<

$(OBJ_S) : %.o : %.S
	$(AS) $(AS_FLAGS) -o $@ $<

install:
	cp libfixmath.a $(TOOLCHAIN_PREFIX)/lib
	mkdir -p $(TOOLCHAIN_PREFIX)/include/fixmath
	cp fixmath/*.h	$(TOOLCHAIN_PREFIX)/include/fixmath
	cp fixmath.h $(TOOLCHAIN_PREFIX)/include

# Clean rules
.PHONY : clean
clean:
	rm -f $(PROJECT).a $(OBJ)
