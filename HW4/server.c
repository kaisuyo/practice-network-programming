#include <stdio.h>
#include <sys/socket.h>
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
	Node head = setup(fileName);

	int sockfd, rcvBytes, sendBytes;
	socklen_t len;
	char buff[BUFF_SIZE+1]; 
	struct sockaddr_in servaddr, cliaddr;
	//Step 1: Construct socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
		perror("Error: ");
		return 0;
	}
	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(SERV_PORT);
	if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) {
		perror("Error: ");
		return 0;
	}
	//Step 3: Communicate with client

	len = sizeof(cliaddr); 
	Node user = NULL;
	int userIndex = -1;
	while(1) { 
		buff[0] = '\0';
		rcvBytes = recvfrom(sockfd, buff, BUFF_SIZE, 0,(struct sockaddr *) &cliaddr, &len);
		if(rcvBytes < 0){
			perror("Error: ");
			return
			0;
		}
		buff[strlen(buff)] = '\0';
	
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
						sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
						if(sendBytes < 0){
							perror("Error: ");
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
							sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
							if(sendBytes < 0){
								perror("Error: ");
								return 0;
							}
						} else {
							// account is not logined anywhere
							if (user->data.status == ACTIVE) {
								char str[] = "Insert password";
								addToken(str, SEND_PASS);
								sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
								if(sendBytes < 0){
									perror("Error: ");
									return 0;
								}
							} else {
								char str[] = "Account not ready";
								addToken(str, SEND_NAME);
								sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
								if(sendBytes < 0){
									perror("Error: ");
									return 0;
								}
							}
						}
					} else {
						char str[] = "Account not exist";
						addToken(str, SEND_NAME);
						sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
						if(sendBytes < 0){
							perror("Error: ");
							return 0;
						}
					}
					break;
				}
				case SEND_PASS: {
					if (user->data.isSignIn) {
						char str[] = "Account is logined on other client.";
						addToken(str, SEND_NAME);
						sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
						if(sendBytes < 0){
							perror("Error: ");
							return 0;
						}
						break;
					}
					if (strcmp(user->data.password, e[0]) == 0) {
						char str[] = "OK";
						addToken(str, SEND_NEW_PASS);
						sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
						if(sendBytes < 0){
							perror("Error: ");
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
							sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
							if(sendBytes < 0){
								perror("Error: ");
								return 0;
							}
						} else {
							char str[] = "not OK";
							addToken(str, SEND_PASS);
							sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
							if(sendBytes < 0){
								perror("Error: ");
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
						addToken(str, OUT);
						sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
						if(sendBytes < 0){
							perror("Error: ");
							return 0;
						}
						writeFile(head, fileName);
					} else {
						strcpy(str, "Error");
						addToken(str, SEND_NEW_PASS);
						sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
						if(sendBytes < 0){
							perror("Error: ");
							return 0;
						}
					}
					break;
				}
				case OUT: {
					if (strcmp(e[0], "bye") != 0) {
						char str[100] = "use \"bye\" to sign-out current account";
						addToken(str, OUT);
						sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
						if(sendBytes < 0){
							perror("Error: ");
							return 0;
						}
						break;
					}

					char str[100] = "Goodbye ";
					if (userIndex != -1) {
						strcat(str, user->data.username);
						user->data.isSignIn = 0;
						writeFile(head, fileName);
					}
					user = NULL;
					userIndex = -1;
					addToken(str, SEND_NAME);
					sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
					if(sendBytes < 0){
						perror("Error: ");
						return 0;
					}
					break;
				}
				default: {}
			}
			strcpy(buff, "");
		} else {
			US = e[tokenTotal-1][0] - '0';
			char str[] = "Error";
			addToken(str, US);
			sendBytes = sendto(sockfd, str, strlen(str), 0,(struct sockaddr *) &cliaddr, len);
			if(sendBytes < 0){
				perror("Error: ");
				return 0;
			}
		}
	}
	return 0;
}