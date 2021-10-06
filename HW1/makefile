CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: main

main: main.o linkedList.o authenticate.o
	${CC} main.o linkedList.o authenticate.o -o main

main.o: main.c
	${CC} ${CFLAGS} main.c

linkedList.o: linkedList.c
	${CC} ${CFLAGS} linkedList.c

authenticate.o: authenticate.c
	${CC} ${CFLAGS} authenticate.c
clean:
	rm -f *.o *~