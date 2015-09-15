# project.mk

include ../../Makefile.cfg

#PROJ_CFLAGS =
#PROJ_LDFLAGS =
#PROJ_LIBS =

CFLAGS = $(PROJ_CFLAGS)

all: $(PROJNAME)_extra
	mkdir -p cd_root
	psx-gcc -O3 $(CFLAGS) -DEXAMPLES_VMODE=$(EXAMPLES_VMODE) -o $(PROJNAME).elf $(PROJNAME).c $(PROJ_LIBS) $(PROJ_LDFLAGS) 
	elf2exe $(PROJNAME).elf $(PROJNAME).exe
	cp $(PROJNAME).exe cd_root
	systemcnf $(PROJNAME).exe > cd_root/system.cnf
	$(MKISOFS_COMMAND) -o $(PROJNAME).hsf -V $(PROJNAME) -sysid PLAYSTATION cd_root
	mkpsxiso $(PROJNAME).hsf $(PROJNAME).bin $(CDLIC_FILE)
	rm -f $(PROJNAME).hsf
	
clean: $(PROJNAME)_clean_extra
	rm -f $(PROJNAME).bin $(PROJNAME).cue $(PROJNAME).exe $(PROJNAME).elf
	rm -fr cd_root
