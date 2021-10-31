#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "linkedList.h"
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


// _________________--

/* check if user is logined */
int isUserSignIn(Node head, char *username) {
  Node p;
  for(p = head; p != NULL; p = p->next) {
    if (p->data.isSignIn && strcmp(p->data.username, username) == 0) {
      return 1;
    }
  }
  return 0;
}


/* decode string and authenticate if has character not in 0-9 a-z A-Z */
int decode(char *str) {
  if (strlen(str) == 0) {
    return -1;
  }

	char num[100] = "";
	char alph[100] = "";
	char temp[3] = "";
	int i;
	for (i = 0; i < strlen(str); i++) {
    temp[0] = str[i];
    temp[1] = '\0';
		if (str[i] >= '0' && str[i] <= '9') {
			strcat(num, temp);
		} else if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i <= 'Z'])) {
			strcat(alph, temp);
		} else{
      return -1;
    };
	}

  if (strlen(num) == 0 && strlen(alph) != 0) {
    strcpy(str, alph);
    str[strlen(str)] = '\0';
  } else if (strlen(num) != 0 && strlen(alph) == 0) {
    strcpy(str, num);
    str[strlen(str)] = '\0';
  } else {
    strcpy(str, num);
    strcat(str, "\n");
    strcat(str, alph);
    str[strlen(str)] = '\0';
  }

	return 1;
}

/* add a token to tail of string space by | */
void addToken(char *str, UserState us) {
  int len = strlen(str);
  str[len] = '|';
  str[len+1] = '0' + us;
  str[len+2] = '\0';
}