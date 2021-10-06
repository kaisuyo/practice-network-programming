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
  Status status;
}Data;

struct LinkedList{
  Data data;
  struct LinkedList *next;
};
typedef struct LinkedList *Node;


Data createData(char *username, char *password, Status status, int activeTimes, int signInTimes);
Node createNode(Data data);
Node init();
Node addHead(Node head, Data data);
Node addTail(Node head, Data data);
Node delHead(Node head);
Node delTail(Node head);
Node delAt(Node head, int position);
Node updateAt(Node head, int index, Data newData);
Node getNode(Node head, int index);
Data get(Node head, int index);
int search(Node head, Data data);
int searchName(Node head, char *name);
int length(Node head);
void writeFile(Node head, int curIndex, char *fileName);
// void traverse(Node head);
void destroy(Node head);

#endif