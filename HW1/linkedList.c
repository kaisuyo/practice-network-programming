#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "authenticate.h"
#include "linkedList.h"

Data createData(char *username, char *password, Status status, int acticveTimes, int signInTimes) {
  Data data;
  strcpy(data.username, username);
  strcpy(data.password, password);
  data.status = status;
  data.activeTimes = acticveTimes;
  data.signInTimes = signInTimes;
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

Node delHead(Node head) {
  if(head != NULL) {
    Node p = head;
    head = head->next;
    free(p);
  }
  return head;
}

Node delTail(Node head) {
  if (head == NULL || head->next == NULL) {
    return delHead(head);
  }
  Node p = head;
  while(p->next->next != NULL) {
    p = p->next;
  }
  free(p->next);
  return head;
}

Node delAt(Node head, int position) {
  if(position == 0 || head == NULL || head->next == NULL) {
    head = delHead(head);
  }else{
    int k = 1;
    Node p = head;
    while(p->next->next != NULL && k != position) {
      p = p->next;
      ++k;
    }

    if(k != position) {
      head = delTail(head);
    }else{
      Node q = p->next;
      p->next = p->next->next;
      free(q);
    }
  }
  return head;
}

Node updateAt(Node head, int index, Data newData) {
  if (index < length(head)) {
    int k = 0;
    Node p = head;
    while(p->next != NULL && k != index) {
      ++k;
      p = p->next;
    }
    p->data = newData;
  }
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

// void traverse(Node head) {
//   Node p;
//   for(p = head; p != NULL; p = p->next) {
//     printf("%s | %s | %s\n", p->data.username, p->data.password, STATUS[p->data.status]);
//   }
// }

void destroy(Node head) {
  Node tmp;
  while (head != NULL) {
    tmp = head;
    head = head->next;
    free(tmp);
  }
}

int length(Node head) {
  int length = 0;
  for(Node p = head; p != NULL; p = p->next) {
    ++length;
  }
  return length;
}

void writeFile(Node head, int curIndex, char *fileName) {
  FILE *f = fopen(fileName, "w");
  if (f == NULL) {
    exit(0);
  }

  Node p;
  int k = 0;
  for(p = head; p != NULL; p = p->next) {
    if (k == curIndex) {
      fprintf(f, "%s\t%s\t%d\t%d\t%d\t%s\n", p->data.username, p->data.password, p->data.status, p->data.activeTimes, p->data.signInTimes, "*");
    } else {
      fprintf(f, "%s\t%s\t%d\t%d\t%d\n", p->data.username, p->data.password, p->data.status, p->data.activeTimes, p->data.signInTimes);
    }
    k++;
  }
  fclose(f);
}
