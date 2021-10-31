#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__


typedef enum {
  BLOCKED,
  ACTIVE,
  IDLE,
} Status;

typedef struct {
  char username[100];
  char password[100];
  int activeTimes;
  int signInTimes;
  char address[100];
  int isSignIn;
  Status status;
}Data;

struct LinkedList{
  Data data;
  struct LinkedList *next;
};
typedef struct LinkedList *Node;

Data createData(char *username, char *password, Status status, char *address, int activeTimes, int signInTimes, int isSignIn);
Node setup(char *fileName);
Node createNode(Data data);
Node init();
Node addHead(Node head, Data data);
Node addTail(Node head, Data data);
Node updateAt(Node head, int index, Data newData);
Node getNode(Node head, int index);
Data get(Node head, int index);
int search(Node head, Data data);
int searchName(Node head, char *name);
void writeFile(Node head, char *fileName);
void destroy(Node head);

#endif