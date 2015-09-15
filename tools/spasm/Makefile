include ../../Makefile.cfg

OUT = spasm$(EXE_SUFFIX)

OBJS = $(patsubst %.c, %.o, $(wildcard *.c))

$(OUT): $(OBJS)
	$(HOST_CC) $(HOST_CFLAGS) -o $(OUT) $(OBJS)

%.o: %.c
	$(HOST_CC) $(HOST_CFLAGS) -c -o $@ $<

install:
	cp -rv $(OUT) $(TOOLCHAIN_PREFIX)/bin

clean:
	rm -f $(OBJS) $(OUT)
