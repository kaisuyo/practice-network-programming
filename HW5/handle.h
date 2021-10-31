#ifndef __HANDLE__
#define __HANDLE__

typedef enum {SEND_NAME, SEND_PASS, SEND_NEW_PASS, OUT, QUIT} UserState;

void clearBuffer();
int isIpV4(char *str);
int isNumber(char *str);
char **words(char *line, int *total, char *strCut);

void addToken(char *str, UserState us);
int decode(char *str);


#endif