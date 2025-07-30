# TODO debug/release mode builds

CC = gcc
CFLAGS = -pedantic -g -Wall -std=c11 
CCOPT = $(CFLAGS)

all: ctil

ctil: ctil.o zmalloc.o state.o builtins.o

.c.o:
	$(CC) -c $(CCOPT) -I. $<

clean:
	rm -f *.o ctil
