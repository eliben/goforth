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

ctil: input.o ctil.o zmalloc.o state.o builtins.o

.c.o:
	$(CC) -c $(CFLAGS) -I. $<

clean:
	rm -f *.o ctil
