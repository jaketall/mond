#makefile schedule
#Jake Tallman

CC= gcc
CFLAGS= -g -Wall


all: mond example

mond: main.c main.o
	 $(CC) $(CFLAGS) -o mond main.c $(LIBS)


example: example.c example.o
	 $(CC) $(CFLAGS) -o example example.c $(LIBS)
.c.o:
	 gcc -c $(CFLAGS) $< -o $@ $(LIBS)
	    
clean:
	 rm -f example mond *.o
