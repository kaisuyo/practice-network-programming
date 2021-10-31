#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

#include "handle.h"

#define BUFF_SIZE 255

int sockfd;

void sighandler(int signalType) {
  char str[] = "quit";
  addToken(str, QUIT);
  if (send(sockfd, str, strlen(str), 0) < 0) {
    perror("Error");
  };
  close(sockfd);
  exit(1);
}

int main(int argc, char *argv[]) {

  if (argc != 3 || !isIpV4(argv[1]) || !isNumber(argv[2])) {
    printf("INVALID ERROR.!!!");
    return 0;
  }

  int SERV_PORT = atoi(argv[2]);
  char *SERV_ADDR = argv[1];

  struct sockaddr_in servaddr;
  char sendline[BUFF_SIZE+1], recvline[BUFF_SIZE+1];

  // create a socket for client
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Error");
    exit(2);
  }

  // creation of the remote server socket informarion structure
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);
  // connect the client to the server socket
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("Problem in connecting to the server");
    return 0;
  }
  
  /* Signal handler specification structure */
  signal(SIGINT, (void (*)(int))sighandler);

  UserState US = SEND_NAME;

  while(1) {
		sendline[0] = '\0';
    scanf("%[^\n]s", sendline);
    clearBuffer();
    
    switch(US) {
      case SEND_NAME: {
        if (strlen(sendline) == 0) {
          strcpy(sendline, "quit");
          addToken(sendline, QUIT);
          if (send(sockfd, sendline, strlen(sendline), 0) < 0) {
            perror("Error");
            break;
          };
          close(sockfd);
          return 0;
        }
        if (strcmp(sendline, "bye") == 0) {
          addToken(sendline, OUT);
          break;
        }

        addToken(sendline, SEND_NAME);
        break;
      }
      case SEND_PASS: {
        addToken(sendline, SEND_PASS);
        break;
      }
      case SEND_NEW_PASS: {
        addToken(sendline, SEND_NEW_PASS);
				break;
      }
      case OUT: {
        addToken(sendline, OUT);
        break;
      }
      default: {}
    }

    // send and receive data from client socket
    if (send(sockfd, sendline, strlen(sendline), 0) < 0) {
			perror("Error");
			break;
		};

    if (recv(sockfd, recvline, BUFF_SIZE, 0) < 0) {
      perror("Error");
      return 0;
    }
    recvline[strlen(recvline)] = '\0';

    int tokenTotal;
    char **e = words(recvline, &tokenTotal, "|");
		// e[0] : message; e[1]: UserState
		if (tokenTotal != 2) {
			perror("Error");
			return 0;
		}

		US = e[1][0] - '0';
		// printf message
		puts(e[0]);
  }
	close(sockfd);
  return 0;
}