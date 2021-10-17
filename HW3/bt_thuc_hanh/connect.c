#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "connect.h"

int isHostName(char *hostName) {
  struct hostent *he = gethostbyname(hostName);   
  if (he == NULL) return 0;
  return 1;
}

int isIp(char *ip) {
  struct in_addr addr;
  inet_aton(ip, &addr);
	struct hostent* host = gethostbyaddr(((const char*)&addr), sizeof(addr), AF_INET);

  if(host == NULL) return 0;
  return 1;
}

void get_ip(char *hostName, char *ip) {
  struct hostent *he = gethostbyname(hostName);   
  if (he == NULL) {
    printf("not found information\n");
    exit(0);
  }

  struct in_addr **addresses = (struct in_addr **) he->h_addr_list;
  int i;
  for(i = 0; addresses[i] != NULL; i++) {
    strcpy(ip , inet_ntoa(*addresses[i]));
    return;
  }
}

void get_hostName(char *ip, char *hostName) {
  struct in_addr addr;
  inet_aton(ip, &addr);
	struct hostent* host = gethostbyaddr(((const char*)&addr), sizeof(addr), AF_INET);

  if(host == NULL) {
    printf("not found information\n");
    exit(0);
  }
  
  strcpy(hostName, host->h_name);
}

char **numbers(char *line, int *total) {
  char **w = (char**)malloc(sizeof(char)*100);
  *w = (char*)malloc(sizeof(char)*100);

  int n = 0;
  char * token = strtok(line, ".");
  while( token != NULL ) {
    *(w + n++)  = token;
    token = strtok(NULL, ".");
  }
  *total = n;
  return w;
}

int isNumber(char *str) {
  int i, len = strlen(str);
  for (i = 0; i < len; i++) {
    if (str[i] < '0' || str[i] > '9') return 0;
  }
  return 1;
}

// check input string is a IP v4 
int isIpV4(char *str) {
  char temp[100];
  strcpy(temp, str);
  int total;
  char **e = numbers(temp, &total);

  if (total != 4) {
    return 0;
  };

  int i;
  for (i = 0; i < total; i++) {
    if (!isNumber(e[i]) || atoi(e[i]) < 0 || atoi(e[i]) > 255) return 0;
  }

  return 1;
}
