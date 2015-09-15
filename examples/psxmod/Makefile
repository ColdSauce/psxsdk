PROJNAME = psxmod
PROJ_LIBS = -lmodplay -lm -ladpcm

include ../project.mk

SAMPLES = $(patsubst mods/%.mod, mods/%.smp, $(wildcard mods/*.mod))

$(PROJNAME)_extra: $(SAMPLES)
	mkdir -p cd_root
	cp -r mods cd_root

$(PROJNAME)_clean_extra:
	rm -f mods/*.smp

mods/%.smp: mods/%.mod
	mod4psx $< $@
