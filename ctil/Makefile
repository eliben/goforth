# By default 'make' builds the project in debug mode.
# Run 'make RELEASE=1' to build with full optimizations.

CC = gcc

CFLAGS = -pedantic -Wall -std=c11 
ifeq ($(RELEASE),1)
	CFLAGS += -O3
else
	CFLAGS += -O0 -g
endif

all: ctil

ctil: die.o zmalloc.o input.o state.o builtins.o ctil.o

.c.o:
	$(CC) -c $(CFLAGS) -I. $<

clean:
	rm -f *.o ctil
