CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: resolver

resolver: main.o connect.o linkedList.o authenticate.o userSys.o
	${CC} main.o connect.o linkedList.o authenticate.o userSys.o -o resolver

main.o: main.c
	${CC} ${CFLAGS} main.c

linkedList.o: linkedList.c
	${CC} ${CFLAGS} linkedList.c

authenticate.o: authenticate.c
	${CC} ${CFLAGS} authenticate.c

connect.o: connect.c
	${CC} ${CFLAGS} connect.c
	
userSys.o: userSys.c
	${CC} ${CFLAGS} userSys.c

clean:
	rm -f *.o *~