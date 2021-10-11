#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "connect.h"

enum {HOSTNAME, IP};

int handleInput(char *arg) {
  if (strlen(arg) != 1) {
    printf("INVALID INPUT!!!\n");
    exit(0);
  }

  if (atoi(arg) == 1) return IP;
  if (atoi(arg) == 2) return HOSTNAME;
  
  printf("INVALID INPUT!!!\n");
  exit(0);
}

int main(int argc , char *argv[]) {
	if(argc != 3) {
    printf("INVALID INPUT!!!\n");
    exit(0);
  }

  int option = handleInput(argv[1]);
  int ipV4 = isIpV4(argv[2]);

  if (option == IP && ipV4) {
    char hostName[100];
    get_hostName(argv[2], hostName);
    printf("Official name: %s\n", hostName);
    return 0;
  };

  if(option == HOSTNAME && !ipV4) {
    char ip[100];
    get_ip(argv[2], ip);
    printf("Official IP: %s\n", ip);
    return 0;
  }

  if ((option == HOSTNAME && ipV4) || (option == IP && !ipV4)) {
    printf("wrong parameter\n");
    exit(0);
  }

  return 0;
}
