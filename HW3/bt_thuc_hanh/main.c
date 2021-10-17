#include <stdio.h>
#include "linkedList.h"
#include "userSys.h"

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("No input file.\n");
    return -1;
  }

  Node head = setup(argv[1]);

  while (1) {
    int choice = menu();

    if (choice == 1) {
      Register(head, argv[1]);
    } else if (choice == 2) {
      Activate(head, argv[1]);
    } else if (choice == 3) {
      SignIn(head, argv[1]);
    } else if (choice == 4) {
      Search(head);
    } else if (choice == 5) {
      ChangePassword(head, argv[1]);
    } else if (choice == 6) {
      SignOut(head, argv[1]);
    } else if (choice == 7) {
      HomePageWithDomainName(head);
    } else if (choice == 8) {
      HomePageWithIPAddress(head);
    } else break;

  }
  destroy(head);
  return 0;
}