#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#include "handle.h"
#include "linkedList.h"

#define BUFF_SIZE 255

struct ThreadArgs {
	int connefd;
	char fileName[100];
	Node head;
};

void *ThreadMain(void *threadArgs) {
	int connefd, n, userIndex = -1;
	char buff[BUFF_SIZE+1], fileName[BUFF_SIZE];
	Node user = NULL;
	int sendBytes;

	pthread_detach(pthread_self());
	connefd = ((struct ThreadArgs *) threadArgs)->connefd;
	strcpy(fileName, ((struct ThreadArgs *) threadArgs)->fileName);
	Node head = ((struct ThreadArgs *) threadArgs)->head;
	free(threadArgs);

	while (1) {
		buff[0] = '\0';
		n = recv(connefd, buff, BUFF_SIZE, 0);
		if (n < 0 ) {
			// perror("Error");
			break;
		}
		// printf("String received from add resent to the client:\n");
		buff[strlen(buff)] = '\0';
		// puts(buff);
		int tokenTotal;
		/* Username and password are separated by |
			tokenTotal == 1 : buff is user name 
			tokenTotal == 2 : buff is password|username
		*/
		char **e = words(buff, &tokenTotal, "|");

		UserState US;
		// e[0]: string sended by client; e[1]: UserState
		if (tokenTotal == 2) {
			US = e[1][0] - '0';
			
			switch (US) {
				case SEND_NAME: {
					userIndex = searchName(head, e[0]);

					if (userIndex == -1) {
						char str[] = "Account not exist";
						addToken(str, SEND_NAME);
						sendBytes = send(connefd, str, strlen(str), 0);
						if(sendBytes < 0){
							perror("Error");
							close(connefd);
							return 0;
						}
						break;
					}

					user = getNode(head, userIndex);
					if (strcmp(user->data.username, e[0]) == 0) {
						// account is logined on other client
						if (user->data.isSignIn) {
							char str[] = "Account is logined on other client.";
							addToken(str, SEND_NAME);
							sendBytes = send(connefd, str, strlen(str), 0);
							if(sendBytes < 0){
								perror("Error");
								close(connefd);
								return 0;
							}
						} else {
							// account is not logined anywhere
							if (user->data.status == ACTIVE) {
								char str[] = "Insert password";
								addToken(str, SEND_PASS);
								sendBytes = send(connefd, str, strlen(str), 0);
								if(sendBytes < 0){
									perror("Error");
									close(connefd);
									return 0;
								}
							} else {
								char str[] = "Account not ready";
								addToken(str, SEND_NAME);
								sendBytes = send(connefd, str, strlen(str), 0);
								if(sendBytes < 0){
									perror("Error");
									close(connefd);
									return 0;
								}
							}
						}
					} else {
						char str[] = "Account not exist";
						addToken(str, SEND_NAME);
						sendBytes = send(connefd, str, strlen(str), 0);
						if(sendBytes < 0){
							perror("Error");
							close(connefd);
							return 0;
						}
					}
					break;
				}
				case SEND_PASS: {
					if (user->data.isSignIn) {
						char str[] = "Account is logined on other client.";
						addToken(str, SEND_NAME);
						sendBytes = send(connefd, str, strlen(str), 0);
						if(sendBytes < 0){
							perror("Error");
							close(connefd);
							return 0;
						}
						break;
					}
					if (strcmp(user->data.password, e[0]) == 0) {
						char str[] = "OK";
						addToken(str, SEND_NEW_PASS);
						sendBytes = send(connefd, str, strlen(str), 0);
						if(sendBytes < 0){
							perror("Error");
							close(connefd);
							return 0;
						}
						user->data.isSignIn = 1;
						user->data.signInTimes = 3;
						writeFile(head, fileName);

					} else {
						user->data.signInTimes--;
						if (user->data.signInTimes <= 0) {
							user->data.status = BLOCKED;
							char str[] = "Account is blocked";
							addToken(str, SEND_NAME);
							sendBytes = send(connefd, str, strlen(str), 0);
							if(sendBytes < 0){
								perror("Error");
								close(connefd);
								return 0;
							}
						} else {
							char str[] = "not OK";
							addToken(str, SEND_PASS);
							sendBytes = send(connefd, str, strlen(str), 0);
							if(sendBytes < 0){
								perror("Error");
								close(connefd);
								return 0;
							}
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
						sendBytes = send(connefd, str, strlen(str), 0);
						if(sendBytes < 0){
							perror("Error");
							close(connefd);
							return 0;
						}
						writeFile(head, fileName);
					} else {
						strcpy(str, "Error");
						addToken(str, SEND_NEW_PASS);
						sendBytes = send(connefd, str, strlen(str), 0);
						if(sendBytes < 0){
							perror("Error");
							close(connefd);
							return 0;
						}
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
					sendBytes = send(connefd, str, strlen(str), 0);
					if(sendBytes < 0){
						perror("Error");
						close(connefd);
						return 0;
					}
					break;
				}
				case QUIT: {
					close(connefd);
					break;
				}
				default: {}
			}
			strcpy(buff, "");
		} else {
			US = e[tokenTotal-1][0] - '0';
			char str[] = "Error";
			addToken(str, US);
			sendBytes = send(connefd, str, strlen(str), 0);
			if(sendBytes < 0){
				perror("Error");
				// close(connefd);
				return 0;
			}
		}
	}
	close(connefd);
	return NULL;
};

int main(int argc, char *argv[]) {
	if(argc != 2 || !isNumber(argv[1])) {
    printf("INVALID INPUT!!!\n");
    exit(0);
  }
	// setup argument input
	char fileName[] = "nguoidung.txt";
	int SERV_PORT = atoi(argv[1]);
	Node head = setup(fileName);
	// setup thread
	pthread_t threadID;
	struct ThreadArgs *threadArgs;

	int listenfd, connefd;
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
	// Node user = NULL;
	// int userIndex = -1;

	while (1) {
		// int sendBytes;
		// accept a connection request -> return a File Descriptor
		connefd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
		/* Create separate memory for client argument */
		threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
		threadArgs->connefd = connefd;
		threadArgs->head = head;
		strcpy(threadArgs->fileName, fileName);
		if (pthread_create(&threadID, NULL, ThreadMain, (void *)threadArgs) != 0) {
			close(connefd);
		};
		// printf("Received request...\n");
		// Send and receive data from the FD

			// while (1) {
			// 	buff[0] = '\0';
			// 	n = recv(connefd, buff, BUFF_SIZE, 0);
			// 	if (n < 0 ) {
			// 		// perror("Error");
			// 		break;
			// 	}
			// 	// printf("String received from add resent to the client:\n");
			// 	buff[strlen(buff)] = '\0';
			// 	// puts(buff);
			// 	int tokenTotal;
			// 	/* Username and password are separated by |
			// 		tokenTotal == 1 : buff is user name 
			// 		tokenTotal == 2 : buff is password|username
			// 	*/
			// 	char **e = words(buff, &tokenTotal, "|");

			// 	UserState US;
			// 	// e[0]: string sended by client; e[1]: UserState
			// 	if (tokenTotal == 2) {
			// 		US = e[1][0] - '0';
					
			// 		switch (US) {
			// 			case SEND_NAME: {
			// 				userIndex = searchName(head, e[0]);

			// 				if (userIndex == -1) {
			// 					char str[] = "Account not exist";
			// 					addToken(str, SEND_NAME);
			// 					sendBytes = send(connefd, str, strlen(str), 0);
			// 					if(sendBytes < 0){
			// 						perror("Error");
			// 						close(connefd);
			// 						return 0;
			// 					}
			// 					break;
			// 				}

			// 				user = getNode(head, userIndex);
			// 				if (strcmp(user->data.username, e[0]) == 0) {
			// 					// account is logined on other client
			// 					if (user->data.isSignIn) {
			// 						char str[] = "Account is logined on other client.";
			// 						addToken(str, SEND_NAME);
			// 						sendBytes = send(connefd, str, strlen(str), 0);
			// 						if(sendBytes < 0){
			// 							perror("Error");
			// 							close(connefd);
			// 							return 0;
			// 						}
			// 					} else {
			// 						// account is not logined anywhere
			// 						if (user->data.status == ACTIVE) {
			// 							char str[] = "Insert password";
			// 							addToken(str, SEND_PASS);
			// 							sendBytes = send(connefd, str, strlen(str), 0);
			// 							if(sendBytes < 0){
			// 								perror("Error");
			// 								close(connefd);
			// 								return 0;
			// 							}
			// 						} else {
			// 							char str[] = "Account not ready";
			// 							addToken(str, SEND_NAME);
			// 							sendBytes = send(connefd, str, strlen(str), 0);
			// 							if(sendBytes < 0){
			// 								perror("Error");
			// 								close(connefd);
			// 								return 0;
			// 							}
			// 						}
			// 					}
			// 				} else {
			// 					char str[] = "Account not exist";
			// 					addToken(str, SEND_NAME);
			// 					sendBytes = send(connefd, str, strlen(str), 0);
			// 					if(sendBytes < 0){
			// 						perror("Error");
			// 						close(connefd);
			// 						return 0;
			// 					}
			// 				}
			// 				break;
			// 			}
			// 			case SEND_PASS: {
			// 				if (user->data.isSignIn) {
			// 					char str[] = "Account is logined on other client.";
			// 					addToken(str, SEND_NAME);
			// 					sendBytes = send(connefd, str, strlen(str), 0);
			// 					if(sendBytes < 0){
			// 						perror("Error");
			// 						close(connefd);
			// 						return 0;
			// 					}
			// 					break;
			// 				}
			// 				if (strcmp(user->data.password, e[0]) == 0) {
			// 					char str[] = "OK";
			// 					addToken(str, SEND_NEW_PASS);
			// 					sendBytes = send(connefd, str, strlen(str), 0);
			// 					if(sendBytes < 0){
			// 						perror("Error");
			// 						close(connefd);
			// 						return 0;
			// 					}
			// 					user->data.isSignIn = 1;
			// 					user->data.signInTimes = 3;
			// 					writeFile(head, fileName);

			// 				} else {
			// 					user->data.signInTimes--;
			// 					if (user->data.signInTimes <= 0) {
			// 						user->data.status = BLOCKED;
			// 						char str[] = "Account is blocked";
			// 						addToken(str, SEND_NAME);
			// 						sendBytes = send(connefd, str, strlen(str), 0);
			// 						if(sendBytes < 0){
			// 							perror("Error");
			// 							close(connefd);
			// 							return 0;
			// 						}
			// 					} else {
			// 						char str[] = "not OK";
			// 						addToken(str, SEND_PASS);
			// 						sendBytes = send(connefd, str, strlen(str), 0);
			// 						if(sendBytes < 0){
			// 							perror("Error");
			// 							close(connefd);
			// 							return 0;
			// 						}
			// 					}
			// 					writeFile(head, fileName);
			// 				}
			// 				break;
			// 			}
			// 			case SEND_NEW_PASS: {
			// 				char str[100] = "";
			// 				strcpy(str, e[0]);
			// 				int result = decode(str);
			// 				if (result == 1) {
			// 					strcpy(user->data.password, e[0]);
			// 					addToken(str, OUT);
			// 					sendBytes = send(connefd, str, strlen(str), 0);
			// 					if(sendBytes < 0){
			// 						perror("Error");
			// 						close(connefd);
			// 						return 0;
			// 					}
			// 					writeFile(head, fileName);
			// 				} else {
			// 					strcpy(str, "Error");
			// 					addToken(str, SEND_NEW_PASS);
			// 					sendBytes = send(connefd, str, strlen(str), 0);
			// 					if(sendBytes < 0){
			// 						perror("Error");
			// 						close(connefd);
			// 						return 0;
			// 					}
			// 				}
			// 				break;
			// 			}
			// 			case OUT: {
			// 				if (strcmp(e[0], "bye") != 0) {
			// 					char str[100] = "use \"bye\" to sign-out current account";
			// 					addToken(str, OUT);
			// 					sendBytes = send(connefd, str, strlen(str), 0);
			// 					if(sendBytes < 0){
			// 						perror("Error");
			// 						close(connefd);
			// 						return 0;
			// 					}
			// 					break;
			// 				}

			// 				char str[100] = "Goodbye ";
			// 				if (userIndex != -1) {
			// 					strcat(str, user->data.username);
			// 					user->data.isSignIn = 0;
			// 					writeFile(head, fileName);
			// 				}
			// 				user = NULL;
			// 				userIndex = -1;
			// 				addToken(str, SEND_NAME);
			// 				sendBytes = send(connefd, str, strlen(str), 0);
			// 				if(sendBytes < 0){
			// 					perror("Error");
			// 					close(connefd);
			// 					return 0;
			// 				}
			// 				break;
			// 			}
			// 			case QUIT: {
			// 				close(connefd);
			// 				break;
			// 			}
			// 			default: {}
			// 		}
			// 		strcpy(buff, "");
			// 	} else {
			// 		US = e[tokenTotal-1][0] - '0';
			// 		char str[] = "Error";
			// 		addToken(str, US);
			// 		sendBytes = send(connefd, str, strlen(str), 0);
			// 		if(sendBytes < 0){
			// 			perror("Error");
			// 			// close(connefd);
			// 			return 0;
			// 		}
			// 	}
			// 	// 
			// 	// if (send(connefd, buff, n, 0) < 0) {
			// 	// 	perror("Erorr: ");
			// 	// 	exit(0);
			// 	// };
			// }
		// close the file descriptor
		// close(connefd);
	}
	// close the listening socket
	close(listenfd);
	return 0;
}