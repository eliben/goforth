CC = gcc
CFLAGS = -pedantic -g -Wall -std=c11 
CCOPT = $(CFLAGS)

all: ctil

ctil: ctil.o

.c.o:
	$(CC) -c $(CCOPT) $<

clean:
	rm -f *.o ctil
