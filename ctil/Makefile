CC = gcc
CFLAGS = -pedantic -g -Wall -std=c11 
CCOPT = $(CFLAGS)

all: ctil

ctil: ctil.o zmalloc.o

.c.o:
	$(CC) -c $(CCOPT) -I. $<

clean:
	rm -f *.o ctil
