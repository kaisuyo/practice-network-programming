#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>

#include "handle.h"

#define BUFF_SIZE 255

struct ThreadArgs {
	int connefd;
	int listenfd;
	char fileName[100];
	Account *A;
	int count;
};



void *ThreadMain(void *threadArgs) {
	char buff[BUFF_SIZE], fileName[BUFF_SIZE];
	int connefd;

	pthread_detach(pthread_self());

	connefd = ((struct ThreadArgs *) threadArgs)->connefd;
	strcpy(fileName, ((struct ThreadArgs *) threadArgs)->fileName);
	Account *A, currentAcc;
	strcpy(currentAcc.name, "");
	strcpy(currentAcc.pass, "");

	int count = ((struct ThreadArgs *) threadArgs)->count;
	A = ((struct ThreadArgs *) threadArgs)->A;
	free(threadArgs);
	
	while (1) {
		buff[0] = '\0';
		if (recv(connefd, buff, BUFF_SIZE, 0) < 0 ) {
			close(connefd);
			continue;
		}

		buff[strlen(buff)] = '\0';
		int total;
		char **e = words(buff, &total, "|");
		if (total != 2) {
			close(connefd);
			continue;
		}

		UserState US = e[1][0] - '0';
		switch(US) {
			case SEND_NAME: {
				currentAcc = findUserName(A, count, e[0]);
				if (strlen(currentAcc.name) != 0) {
					if (currentAcc.status == BLOCKED) {
						char str[100];
						strcpy(str, "Account is blocked\nTry other username: ");
						addToken(str, SEND_NAME);
						if (send(connefd, str, strlen(str), 0) < 0) {
							close(connefd);
							return 0;
						}	
						break;
					}
					char str[100];
					strcpy(str, "Insert password: ");
					addToken(str, SEND_PASS);
					if (send(connefd, str, strlen(str), 0) < 0) {
						close(connefd);
						return 0;
					}
				} else {
					char str[100];
					strcpy(str, "Account not exist\nTry other username: ");
					addToken(str, SEND_NAME);
					if (send(connefd, str, strlen(str), 0) < 0) {
						close(connefd);
						return 0;
					}
				}
				break;
			}
			case SEND_PASS: {
				currentAcc = findUserName(A, count, currentAcc.name);
				if (currentAcc.status == BLOCKED) {
					char str[100];
					strcpy(str, "Account is blocked\nTry other username: ");
					addToken(str, SEND_NAME);
					if (send(connefd, str, strlen(str), 0) < 0) {
						close(connefd);
						return 0;
					}	
					break;
				}
				if (strcmp(currentAcc.pass, e[0]) == 0) {
					char str[100];
					strcpy(str, "OK. Continue with other username or quit by Enter: ");
					addToken(str, SEND_NAME);
					if (send(connefd, str, strlen(str), 0) < 0) {
						close(connefd);
						return 0;
					}
				} else {
					int times = --A[findIndex(A, count, currentAcc.name)].tryPassTimes;
					if (times < 0) {
						A[findIndex(A, count, currentAcc.name)].status = 0;
						char str[100];
						strcpy(str, "Account is blocked\nTry other username or quit by Enter: ");
						addToken(str, SEND_NAME);
						if (send(connefd, str, strlen(str), 0) < 0) {
							close(connefd);
							return 0;
						}
						// send data to parent prosses
						writeFile(A, count, fileName);
						break;

					} else {
						char str[100];
						strcpy(str, "Not OK\nTry password again: ");
						addToken(str, SEND_PASS);
						if (send(connefd, str, strlen(str), 0) < 0) {
							close(connefd);
							return 0;
						}
					}
				}
				break;
			}
			case OUT: {
				strcpy(currentAcc.name, "");
				strcpy(currentAcc.pass, "");
				char str[100];
				strcpy(str, "Enter username or quit: ");
				addToken(str, SEND_NAME);
				if (send(connefd, str, strlen(str), 0) < 0) {
					close(connefd);
					return 0;
				}
				break;
			}
			case QUIT: {
				close(connefd);
				break;
			} default: {}
		}
	}
	close(connefd);
	return NULL;
}

int main(int argc, char *argv[]) {
	if(argc != 2 || !isNumber(argv[1])) {
    printf("INVALID INPUT!!!\n");
    exit(0);
  }
	// setup argument input
	int SERV_PORT = atoi(argv[1]);
	// setup thread
	Account A[100];
	char fileName[] = "account.txt";
	int accTotal = setup(A, fileName);
	// setup thread
	pthread_t threadID;
	struct ThreadArgs *threadArgs;
	int listenfd, connefd;

	// setup for share memory
	socklen_t clilen;
	
	struct sockaddr_in cliaddr, servaddr;
	// create of server socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error");
		return 0;
	};
	// Preparation of the socket address struct
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	// Bind the socket to the port in address
	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("Error");
		return 0;
	};
	// Listen for connection to the socket
	if (listen(listenfd, 5) < 0) {
		perror("Error");
		return 0;
	};


	// printf("Server running...waiting for connections\n");

	clilen = sizeof(cliaddr);

	while (1) {
		// int sendBytes;
		// accept a connection request -> return a File Descriptor
		connefd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);

		printf("Received request...\n");
		// Send and receive data from the FD
		// use pipe to get return Accounts to parent prosses from child prosses
		/* Create separate memory for client argument */
		threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
		threadArgs->connefd = connefd;
		threadArgs->listenfd = listenfd;
		threadArgs->count = accTotal;
		((struct ThreadArgs *) threadArgs)->A = A;
		strcpy(threadArgs->fileName, fileName);
		if (pthread_create(&threadID, NULL, ThreadMain, (void *)threadArgs) != 0) {
			close(connefd);
		};
	}
	close(listenfd);
	return 0;
}