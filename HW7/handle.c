#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "handle.h"

/* divide the string to substrings by strCut
  total: total of substirngs
*/
char **words(char *line, int *total, char *strCut) {
  char **w = (char**)malloc(sizeof(char)*100);
  *w = (char*)malloc(sizeof(char)*100);

  int n = 0;
  char * token = strtok(line, strCut);
  while( token != NULL ) {
    *(w + n++)  = token;
    token = strtok(NULL, strCut);
  }
  *total = n;
  return w;
}

/* Check if string converts to number */
int isNumber(char *str) {
  int i, len = strlen(str);
  for (i = 0; i < len; i++) {
    if (str[i] < '0' || str[i] > '9') return 0;
  }
  return 1;
}

/* Check if string is ipv4
Ex:  127.0.0.1 */
int isIpV4(char *str) {
  char temp[100];
  strcpy(temp, str);
  int total;
  char **e = words(temp, &total, ".");

  if (total != 4) {
    return 0;
  };

  int i;
  for (i = 0; i < total; i++) {
    if (!isNumber(e[i]) || atoi(e[i]) < 0 || atoi(e[i]) > 255) return 0;
  }

  return 1;
}

void clearBuffer() {
  int c;
  do {
    c = getchar();
  } while (c != '\n' && c != EOF);
}

/* add a token to tail of string space by | */
void addToken(char *str, UserState us) {
  int len = strlen(str);
  str[len] = '|';
  str[len+1] = '0' + us;
  str[len+2] = '\0';
}

int setup(Account A[], char *fileName) {
  int i = 0;
  FILE *f = fopen(fileName, "r");
  if (f == NULL) {
    printf("File not found");
    exit(0);
  }

  while (!feof(f)) {
    char str[100];
    int total;
    fgets(str, 100, f);
    char **e = words(str, &total, "\t\n ");
    if (total != 3) continue;
    strcpy(A[i].name, e[0]);
    strcpy(A[i].pass, e[1]);
    A[i].status = atoi(e[2]);
    A[i].tryPassTimes = 3;
    i++;
  }
  fclose(f);
  return i;
}

Account findUserName(Account A[], int total, char *name) {
  Account a;
  strcpy(a.name, "");
  strcpy(a.pass, "");
  int i;
  for (i = 0; i < total; i++) {
    if (strcmp(A[i].name, name) == 0) {
      strcpy(a.name, A[i].name);
      strcpy(a.pass, A[i].pass);
      a.status = A[i].status;
      a.tryPassTimes = A[i].tryPassTimes;
    }
  }
  return a;
}

int findIndex(Account A[], int total, char *name) {
  int i;
  for (i = 0; i < total; i++) {
    if (strcmp(A[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

void writeFile(Account A[], int total, char *fileName) {
  int i;
  FILE *f = fopen(fileName, "w");
  if (f == NULL) {
    printf("Write to file failed");
    exit(0);
  }
  for (i = 0; i < total; i++) {
    fprintf(f, "%s\t%s\t%d\n", A[i].name, A[i].pass, A[i].status);
  }
  fclose(f);
}

// shared memories between prosess

void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}