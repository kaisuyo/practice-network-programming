CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client server

client: client.o handle.o
	${CC} client.o handle.o -o client

client.o: client.c
	${CC} ${CFLAGS} client.c

server: server.o handle.o
	${CC} -pthread server.o handle.o -o server

server.o: server.c
	${CC} ${CFLAGS} server.c

handle.o: handle.c
	${CC} ${CFLAGS} handle.c

clean:
	rm -f *.o *~