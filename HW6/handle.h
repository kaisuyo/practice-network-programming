#ifndef __HANDLE_H__
#define __HANDLE_H__

typedef enum {
  BLOCKED,
  ACTIVE
} Status;

typedef enum {
  SEND_NAME,
  SEND_PASS,
  OUT,
  QUIT
} UserState;

typedef struct {
  char name[100];
  char pass[100];
  Status status;
  int tryPassTimes;
} Account;

char **words(char *line, int *total, char *strCut);
int isNumber(char *str);
int isIpV4(char *str);
void clearBuffer();
void addToken(char *str, UserState us);
int setup(Account A[], char *fileName);
Account findUserName(Account A[], int total, char *name);
int findIndex(Account A[], int total, char *name);
void writeFile(Account A[], int total, char *fileName);
// share memory between prossess
void* create_shared_memory(size_t size);

#endif