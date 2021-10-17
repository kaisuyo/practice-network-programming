#include "linkedList.h"

#ifndef __USERSYS__
#define __USERSYS__

Node setup(char *fileName);
void Register(Node head, char *fileName);
void Activate(Node head, char *fileName);
void SignIn(Node head, char *fileName);
void Search(Node head);
void ChangePassword(Node head, char *fileName);
void SignOut(Node head, char *fileName);
void HomePageWithDomainName(Node head);
void HomePageWithIPAddress(Node head);
int menu();

#endif