#ifndef __AUTHENTICATE_H__
#define __AUTHENTICATE_H__

void clearBuffer();
void err(char *str);
void warning(char *str);
void success(char *str);
int isInvalid(char *str);
int isTrueCode(char *code);
char **words(char *line, int *total);

#endif

