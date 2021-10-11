CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: resolver

resolver: main.o connect.o
	${CC} main.o connect.o -o resolver

main.o: main.c
	${CC} ${CFLAGS} main.c

connect.o: connect.c
	${CC} ${CFLAGS} connect.c

clean:
	rm -f *.o *~