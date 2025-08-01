# Run 'make DEBUG=1' to build for debugging.

CC = gcc

CFLAGS = -pedantic -Wall -std=c11 
ifeq ($(DEBUG),1)
	CFLAGS += -O0 -g
else
	CFLAGS += -O3
endif

all: ctil

ctil: input.o ctil.o zmalloc.o state.o builtins.o

.c.o:
	$(CC) -c $(CFLAGS) -I. $<

clean:
	rm -f *.o ctil
