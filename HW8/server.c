#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "handle.h"
#include "linkedList.h"

#define BUFF_SIZE 255

int main(int argc, char *argv[]) {
	if(argc != 2 || !isNumber(argv[1])) {
    printf("INVALID INPUT!!!\n");
    exit(0);
  }

	char fileName[] = "nguoidung.txt";

	int SERV_PORT = atoi(argv[1]);
	Node head = setup("nguoidung.txt");

	int o = 1;
	int masterSocket, len, newSocket, clients[100], clientsMax = 100;
	int sockfd, sdMax;
	fd_set readfds;

	struct sockaddr_in address;  
	char buff[BUFF_SIZE+1]; 
	struct sockaddr_in servaddr, cliaddr;

	int i;
	for (i = 0; i < clientsMax; i++) {  
    clients[i] = 0;  
  }  

	//Step 1: Construct socket
	if((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
		perror("Error: ");
		return 0;
	}
	// set master socket to allow multiple connect
	if((setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&o, sizeof(o))) < 0) { 
		perror("Error: ");
		return 0;
	}
	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(SERV_PORT);
	if(bind(masterSocket, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		perror("Error: ");
		return 0;
	}
	//Step 3: listen
	if (listen(masterSocket, 10) < 0) {
		perror("Error: ");
		return 0;
	}
	len = sizeof(cliaddr); 

	Node user = NULL;
	int userIndex = -1;

	while(1) { 
		FD_ZERO(&readfds);
		FD_SET(masterSocket, &readfds);
		sdMax = masterSocket;

		int i;
		for (i = 0; i < clientsMax; i++) {
			sockfd = clients[i];
			if (sockfd > 0) {
				FD_SET(sockfd, &readfds);
			}
			if (sockfd > sdMax) {
				sdMax = sockfd;
			}
		}

		select(sdMax+1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(masterSocket, &readfds)) {
			newSocket = accept(masterSocket, (struct sockaddr*)&address, (socklen_t*)&len);
			if (newSocket < 0) {
				perror("Error: ");
				return 0;
			}

			int i;
			for (i = 0; i < clientsMax; i++) {
				if (clients[i] == 0) {
					clients[i] = newSocket;
					break;
				}
			}
		}

		int index;
		for (index = 0; index < clientsMax; index++) {
			sockfd = clients[index];
			if (FD_ISSET(sockfd, &readfds)) {
				if (recv(sockfd, buff, BUFF_SIZE, 0) <= 0) {
					if (user != NULL && user->data.isSignIn) {
						user->data.isSignIn = 0;
						user = NULL;
						userIndex = -1;
					}
					close(sockfd);
					clients[index] = 0;
					continue;
				}

				int tokenTotal;
				UserState US;
				char **e = words(buff, &tokenTotal, "|");

				if (tokenTotal == 2) {
					US = e[1][0] - '0';
					
					switch (US) {
						case SEND_NAME: {
							userIndex = searchName(head, e[0]);

							if (userIndex == -1) {
								char str[] = "Account not exist";
								addToken(str, SEND_NAME);
								send(sockfd, str, strlen(str), 0);
								break;
							}

							user = getNode(head, userIndex);
							if (strcmp(user->data.username, e[0]) == 0) {
								// account is logined on other client
								if (user->data.isSignIn) {
									char str[] = "Account is logined on other client.";
									addToken(str, SEND_NAME);
									send(sockfd, str, strlen(str), 0);
								} else {
									// account is not logined anywhere
									if (user->data.status == ACTIVE) {
										char str[] = "Insert password";
										addToken(str, SEND_PASS);
										send(sockfd, str, strlen(str), 0);
									} else {
										char str[] = "Account not ready";
										addToken(str, SEND_NAME);
										send(sockfd, str, strlen(str), 0);
									}
								}
							} else {
								char str[] = "Account not exist";
								addToken(str, SEND_NAME);
								send(sockfd, str, strlen(str), 0);
							}
							break;
						}
						case SEND_PASS: {
							if (user->data.isSignIn) {
								char str[] = "Account is logined on other client.";
								addToken(str, SEND_NAME);
								send(sockfd, str, strlen(str), 0);
								break;
							}
							if (user->data.status != ACTIVE) {
								char str[] = "Account not ready";
								addToken(str, SEND_NAME);
								send(sockfd, str, strlen(str), 0);
								break;
							}
							if (strcmp(user->data.password, e[0]) == 0) {
								char str[] = "OK";
								addToken(str, SEND_NEW_PASS);
								send(sockfd, str, strlen(str), 0);
								user->data.isSignIn = 1;
								user->data.signInTimes = 3;
								writeFile(head, fileName);

							} else {
								user->data.signInTimes--;
								if (user->data.signInTimes <= 0) {
									user->data.status = BLOCKED;
									char str[] = "Account is blocked";
									addToken(str, SEND_NAME);
									send(sockfd, str, strlen(str), 0);
								} else {
									char str[] = "not OK";
									addToken(str, SEND_PASS);
									send(sockfd, str, strlen(str), 0);
								}
								writeFile(head, fileName);
							}
							break;
						}
						case SEND_NEW_PASS: {
							char str[100] = "";
							strcpy(str, e[0]);
							int result = decode(str);
							if (result == 1) {
								strcpy(user->data.password, e[0]);
								addToken(str, SEND_NAME);
								send(sockfd, str, strlen(str), 0);
								user->data.isSignIn = 0;
								writeFile(head, fileName);
							} else {
								strcpy(str, "Error");
								addToken(str, SEND_NEW_PASS);
								send(sockfd, str, strlen(str), 0);
							}
							break;
						}
						case OUT: {
							char str[100] = "Goodbye ";
							if (userIndex != -1) {
								strcat(str, user->data.username);
								user->data.isSignIn = 0;
								writeFile(head, fileName);
							}
							user = NULL;
							userIndex = -1;
							addToken(str, SEND_NAME);
							send(sockfd, str, strlen(str), 0);
							break;
						}
						default: {}
					}
					strcpy(buff, "");
				} else {
					US = e[tokenTotal-1][0] - '0';
					char str[] = "Error";
					addToken(str, US);
					send(sockfd, str, strlen(str), 0);
				}
			}
		}
	}
	return 0;
}