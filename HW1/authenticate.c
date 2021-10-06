#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#include "authenticate.h"

void clearBuffer() {
  int c;
  do {
    c = getchar();
  } while (c != '\n' && c != EOF);
}

char **words(char *line, int *total) {
  char **w = (char**)malloc(sizeof(char)*100);
  *w = (char*)malloc(sizeof(char)*100);

  int n = 0;
  char * token = strtok(line, " \t\n");
  while( token != NULL ) {
    *(w + n++)  = token;
    token = strtok(NULL, " \t\n");
  }
  *total = n;
  return w;
}

int isInvalid(char *str) {
  int len = strlen(str), i;

  if (len == 0) return 1;

  for (i = 0; i < len; i++) {
    if (str[i] == ' ' || str[i] < 33 || str[i] > 126) return 1;
  }
  return 0;
}

int isTrueCode(char *code) {
  int len = strlen(code), i;

  if (len != 8) return 0;
  if (code[0] == '0') return 0;
  for (i = 0; i < 8; i++) {
    if (code[i] < '0' || code[i] > '9') return 0;
  }
  return 1;
}

void err(char *str) {
  printf("\033[0;31m");
  printf("\n%s\n", str);
  printf("\033[0m");
}

void success(char *str) {
  printf("\033[0;32m");
  printf("\n%s\n", str);
  printf("\033[0m");
}

void warning(char *str) {
  printf("\033[0;33m");
  printf("\n%s\n", str);
  printf("\033[0m");
}
