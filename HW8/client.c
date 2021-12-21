#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "handle.h"

#define BUFF_SIZE 255

int main(int argc, char *argv[]) {
	if(argc != 3 || !isIpV4(argv[1]) || !isNumber(argv[2])) {
    printf("INVALID INPUT!!!\n");
    exit(0);
  }

	int SERV_PORT = atoi(argv[2]);
	char *SERV_ADDR = argv[1];

	int sockfd;
	struct sockaddr_in servaddr;

	//Step 1: Construct socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error: ");
		return 0;
	} 
	//Step 2: Define the address of the server
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT); 
	servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR); 
	//Step 3: Communicate with server
	
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("Problem in connecting to the server");
    return 0;
  }
	UserState US = SEND_NAME;
	printf("Enter username: ");
	/* buff is input string join with "[|token1|token2...]" */
	char buff[BUFF_SIZE+1]; 
	while(1) {
		// fgets(buff, BUFF_SIZE, stdin);
		buff[0] = '\0';
		scanf("%[^\n]s", buff);
		clearBuffer();
		
		switch (US) {
			case SEND_NAME: {
				if (strlen(buff) == 0) {
					return 0;
				}
				// buff is username
  			addToken(buff, SEND_NAME);
				break;
			}
			case SEND_PASS: {
				// buff is string which is password|username
				addToken(buff, SEND_PASS);
				break;
			}
			case SEND_NEW_PASS: {
				if (strcmp(buff, "bye") == 0) {
					addToken(buff, OUT);
					break;
				}
				addToken(buff, SEND_NEW_PASS);
				break;
			}
			case OUT: {
				addToken(buff, OUT);
				break;
			}
			default:{}
		}
		
		send(sockfd, buff, strlen(buff), 0);
		memset(buff, 0, strlen(buff));
		if (recv(sockfd, buff, BUFF_SIZE, 0) <= 0) {
			close(sockfd);
			return 0;
		}

		int tokenTotal;
		char **e = words(buff, &tokenTotal, "|");
		if (tokenTotal != 2) {
			perror("Error: ");
			close(sockfd);
			return 0;
		}

		US = e[1][0] - '0';
		// printf message
		puts(e[0]);
	}
	return 0;
}