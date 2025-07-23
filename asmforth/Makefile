AS = as
ASFLAGS = --64
LD = ld
LDFLAGS =

all: asmforth

asmforth: asmforth.o
	$(LD) $(LDFLAGS) -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f *.o asmforth
