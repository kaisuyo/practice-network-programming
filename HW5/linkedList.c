#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linkedList.h"
#include "handle.h"

Data createData(char *username, char *password, Status status, char *address, int acticveTimes, int signInTimes, int isSignIn) {
  Data data;
  strcpy(data.username, username);
  strcpy(data.password, password);
  data.status = status;
  strcpy(data.address, address);
  data.activeTimes = acticveTimes;
  data.signInTimes = signInTimes;
  data.isSignIn = isSignIn;
  return data;
}

Node createNode(Data data) {
  Node temp;
  temp = (Node)malloc(sizeof(struct LinkedList));
  temp->next = NULL;
  temp->data = data;
  return temp;
}

Node init() {
  Node head;
  head = NULL;
  return head;
}

Node addHead(Node head, Data data) {
  Node temp = createNode(data); 
  if(head == NULL) {
    head = temp;
  }else{
    temp->next = head;
    head = temp;
  }
  return head;
}

Node addTail(Node head, Data data) {
  Node temp,p;
  temp = createNode(data);
  if(head == NULL) {
    head = temp;
  }
  else{
    p = head;
    while(p->next != NULL) {
      p = p->next;
    }
    p->next = temp;
  }
  return head;
}

Node updateAt(Node head, int index, Data newData) {
  int k = 0;
  Node p = head;
  while(p->next != NULL && k != index) {
    ++k;
    p = p->next;
  }
  p->data = newData;
  return head;
}

Node getNode(Node head, int index) {
  int k = 0;
  Node p = head;
  while(p->next != NULL && k != index) {
    ++k;
    p = p->next;
  }
  return p;
}

Data get(Node head, int index) {
  int k = 0;
  Node p = head;
  while(p->next != NULL && k != index) {
    ++k;
    p = p->next;
  }
  return p->data;
}

int search(Node head, Data data) {
  int position = 0;
  for(Node p = head; p != NULL; p = p->next) {
    if(
      strcmp(p->data.username, data.username) == 0 &&
      strcmp(p->data.password, data.password) == 0
    ) {
      return position;
    }
    ++position;
  }
  return -1;
}

int searchName(Node head, char *name) {
  int position = 0;
  for(Node p = head; p != NULL; p = p->next) {
    if(strcmp(p->data.username, name) == 0) {
      return position;
    }
    ++position;
  }
  return -1;
}

void destroy(Node head) {
  Node tmp;
  while (head != NULL) {
    tmp = head;
    head = head->next;
    free(tmp);
  }
}

void writeFile(Node head, char *fileName) {
  FILE *f = fopen(fileName, "w");
  if (f == NULL) {
    exit(0);
  }

  Node p;
  for(p = head; p != NULL; p = p->next) {
    fprintf(f, "%s\t%s\t%d\t%s\t%d\t%d", p->data.username, p->data.password, p->data.status, p->data.address, p->data.activeTimes, p->data.signInTimes);
    if (p->data.isSignIn && p->data.status != 0) {
      fprintf(f, "\t*");
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

Node setup(char *fileName) {
  FILE *f = fopen(fileName, "r");
  if (f == NULL) {
    printf("Can\'t read input file!\n");
    exit(0);
  }

  Node head = init();
  while(!feof(f)) {
    char line[1000];
    fgets(line, 1000, f);
    if (strlen(line) == 0) continue;
    int total;
    char **e = words(line, &total, " \t\n");
    if (total < 4 || total > 7) continue;
    if (atoi(e[2]) < 0 || atoi(e[2]) > 2) continue;

    int activeTimes, signInTimes, isSignIn;
    
    if(total == 4) {
      activeTimes = (atoi(e[2]) == BLOCKED)? 0:4;
      signInTimes = (atoi(e[2]) == BLOCKED)? 0:3;
      isSignIn = 0;
    } else if (total == 5) {
      if (e[4] == NULL) continue;
      if (strcmp(e[4], "*") == 0) {
        activeTimes = (atoi(e[2]) == BLOCKED)? 0:4;
        signInTimes = (atoi(e[2]) == BLOCKED)? 0:3;
        isSignIn = (atoi(e[2]) == BLOCKED)? 0:1;
      } else continue;
    } else if (total == 6) {
      if (atoi(e[4]) < 0 || atoi(e[4]) > 4 || atoi(e[5]) < 0 || atoi(e[5]) > 3) continue;

      activeTimes = (atoi(e[2]) == BLOCKED)? 0:atoi(e[4]);
      signInTimes = (atoi(e[2]) == BLOCKED)? 0:atoi(e[5]);
      isSignIn = 0;
    } else if (total == 7) {
      if (strcmp(e[6], "*") != 0) continue;
      if (atoi(e[4]) < 0 || atoi(e[4]) > 4 || atoi(e[5]) < 0 || atoi(e[5]) > 3) continue;

      activeTimes = (atoi(e[2]) == BLOCKED)? 0:atoi(e[4]);
      signInTimes = (atoi(e[2]) == BLOCKED)? 0:atoi(e[5]);
      isSignIn = (atoi(e[2]) == BLOCKED)? 0:atoi(e[5]);
    } else continue;
    

    // rieng week4 
    isSignIn = 0;

    
    Data d = createData(e[0], e[1], atoi(e[2]), e[3], activeTimes, signInTimes, isSignIn);
    head = addTail(head, d);
    free(e);
  }
  fclose(f);
  return head;
}