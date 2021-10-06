#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"
#include "authenticate.h"


char STATUS[3][10] = {"Blocked","Active", "Idle"};

Node setup(Node cur, char *fileName) {
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
    char **e = words(line, &total);
    if (total < 3 || total > 6) continue;
    if (atoi(e[2]) < 0 || atoi(e[2]) > 2) continue;

    int activeTimes, signInTimes;
    
    if(total == 3) {
      activeTimes = (atoi(e[2]) == BLOCKED)? 0:4;
      signInTimes = (atoi(e[2]) == BLOCKED)? 0:3;
    } else if (total == 4) {
      if (e[3] == NULL) continue;
      if (strcmp(e[3], "*") == 0) {
        activeTimes = (atoi(e[2]) == BLOCKED)? 0:4;
        signInTimes = (atoi(e[2]) == BLOCKED)? 0:3;
        cur->data = createData(e[0], e[1], atoi(e[2]), activeTimes, signInTimes);
      } else continue;
    } else if (total == 5) {
      if (atoi(e[3]) < 0 || atoi(e[3]) > 4 || atoi(e[4]) < 0 || atoi(e[4]) > 3) continue;

      activeTimes = (atoi(e[2]) == BLOCKED)? 0:atoi(e[3]);
      signInTimes = (atoi(e[2]) == BLOCKED)? 0:atoi(e[4]);
    } else if (total == 6) {
      if (strcmp(e[5], "*") != 0) continue;
      if (atoi(e[3]) < 0 || atoi(e[3]) > 4 || atoi(e[4]) < 0 || atoi(e[4]) > 3) continue;

      activeTimes = (atoi(e[2]) == BLOCKED)? 0:atoi(e[3]);
      signInTimes = (atoi(e[2]) == BLOCKED)? 0:atoi(e[4]);
      cur->data = createData(e[0], e[1], atoi(e[2]), activeTimes, signInTimes);
    } else continue;
    
    Data d = createData(e[0], e[1], atoi(e[2]), activeTimes, signInTimes);
    head = addTail(head, d);
    free(e);
  }
  fclose(f);
  return head;
}

// ___________________________
void Register(Node head, Node cur, char *fileName) {
  char username[100];

  printf("Enter your username: ");
  scanf("%[^\n]s", username);
  clearBuffer();
  if (isInvalid(username)) {
    err("Invalid input username!!!");
    return;
  }

  int index = searchName(head, username);

  if (index != -1) {
    err("Account existed");
    return;
  }

  printf("Enter your password: ");
  char password[100];
  scanf("%[^\n]s", password);
  clearBuffer();
  if (isInvalid(password)) {
    err("Invalid input password!!!");
    return;
  }

  Data d = createData(username, password, IDLE, 4, 3);
  head = addTail(head, d);
  writeFile(head, search(head, cur->data), fileName);
  success("Successful registration. Activation required.");
}

void Activate(Node head, Node cur, char *fileName) {
  char username[100], password[100], code[20];
  printf("Enter your username: ");
  scanf("%[^\n]s", username);
  clearBuffer();
  printf("Enter your password: ");
  scanf("%[^\n]s", password);
  clearBuffer();
  
  int userIndex = searchName(head, username);
  if (userIndex == -1) {
    err("Username dose not exist.");
    return;
  };

  Data temp = get(head, userIndex);
  if (strcmp(temp.password, password) != 0) {
    err("Password is not match.");
    return;
  }

  if (temp.status == ACTIVE) {
    success("Account is activated");
    return;
  }

  if (temp.status == BLOCKED) {
    err("Account is blocked");
    return;
  }

  enterCode: {
    printf("Enter your code: ");
    scanf("%[^\n]s", code);
    clearBuffer();

    if (!isTrueCode(code)) {
      if (temp.activeTimes == 0) {
        Data data = createData(temp.username, temp.password, BLOCKED, 0, temp.signInTimes);
        head = updateAt(head, userIndex, data);
        writeFile(head, search(head, cur->data), fileName);
        err("Activation code is incorrect. Account is blocked");
        return;
      }

      Data newData = createData(temp.username, temp.password, IDLE, --temp.activeTimes, temp.signInTimes);
      head = updateAt(head, userIndex, newData);
      writeFile(head, search(head, cur->data), fileName);
      err("Account is not activated");
      goto enterCode;
    } else {
      Data newData = createData(temp.username, temp.password, ACTIVE, 4, temp.signInTimes);
      head = updateAt(head, userIndex, newData);
      strcpy(cur->data.username, "");
      strcpy(cur->data.password, "");
      writeFile(head, search(head, cur->data), fileName);
      success("Account is activated");
    }
  }
}

void SignIn(Node head, Node cur, char *fileName) {
  char username[100], password[100];
  printf("Enter your username: ");
  scanf("%[^\n]s", username);
  clearBuffer();

  int userIndex = searchName(head, username);
  if (userIndex == -1) {
    err("Cannot find account");
    return;
  }

  Data temp = get(head, userIndex);
  if (temp.status == BLOCKED) {
    err("Account is blocked");
    return;
  }

  signIn: {
    printf("Enter your password: ");
    scanf("%[^\n]s", password);
    clearBuffer();

    if (strcmp(temp.password, password) != 0) {
      if (temp.signInTimes <= 0) {
        Data data = createData(temp.username, temp.password, BLOCKED, temp.activeTimes, 0);
        head = updateAt(head, userIndex, data);
        writeFile(head, search(head, cur->data), fileName);
        err("Password is incorrect. Account is blocked");
        return;
      }

      Data d = createData(temp.username, temp.password, temp.status, temp.activeTimes, --temp.signInTimes);
      head = updateAt(head, userIndex, d);
      writeFile(head, search(head, cur->data), fileName);
      err("Password is incorrect");
      goto signIn;
    } else {
      Data d = createData(temp.username, temp.password, temp.status, temp.activeTimes, 3);
      head = updateAt(head, userIndex, d);
      writeFile(head, userIndex, fileName);
      char s[100] = "Hello ";
      strcat(s, temp.username);
      success(s);
      cur->data = getNode(head, userIndex)->data;
    }
  }
}

void Search(Node head, Node cur) {
  if (strcmp(cur->data.username, "") == 0) {
    err("Account is not sign in");
    return;
  }

  char username[100];
  printf("Enter username: ");
  scanf("%[^\n]s", username);
  clearBuffer();

  int userIndex = searchName(head, username);
  if (userIndex == -1) {
    err("Cannot find account");
    return;
  }

  Data d = get(head, userIndex);
  char s[50] = "Account is ";
  strcat(s, STATUS[d.status]);
  success(s);

}

void ChangePassword(Node head, Node cur, char *fileName) {
  if (strlen(cur->data.username) == 0) {
    err("Account is not sign in");
    return;
  }

  char username[100], password[100], newPassword[100];
  printf("Enter your username: ");
  scanf("%[^\n]s", username);
  clearBuffer();

  int userIndex = searchName(head, username);
  if (userIndex == -1) {
    err("Cannot find account");
    return;
  }

  if (strcmp(username, cur->data.username) != 0) {
    err("Account is not sign in");
    return;
  }

  Data d = get(head, userIndex);
  printf("Enter your old password: ");
  scanf("%[^\n]s", password);
  clearBuffer();

  printf("Enter your new password: ");
  scanf("%[^\n]s", newPassword);
  clearBuffer();

  if (strcmp(password, d.password) != 0) {
    err("Current password is incorrect. Please try again");
    return;
  }

  if (strcmp(password, newPassword) == 0) {
    err("The new password is the same as the old password. Please try again");
    return;
  }

  Data newData = createData(d.username, newPassword, d.status, d.activeTimes, d.signInTimes);
  head = updateAt(head, userIndex, newData);
  writeFile(head, userIndex, fileName);

}

void SignOut(Node head, Node cur, char *fileName) {
  if (strlen(cur->data.username) == 0) {
    err("Account is not sign in");
    return;
  }

  char username[100];
  printf("Enter your username: ");
  scanf("%[^\n]s", username);
  clearBuffer();

  int userIndex = searchName(head, username);
  if (userIndex == -1) {
    err("Cannot find account");
    return;
  }

  if (strcmp(username, cur->data.username) != 0) {
    err("Account is not sign in");
    return;
  }

  strcpy(cur->data.username, "");
  strcpy(cur->data.password, "");
  writeFile(head, search(head, cur->data), fileName);
  char s[100] = "Goodbye ";
  strcat(s, username);
  success(s);
}

int menu() {
  printf("USER MANAGEMENT PROGRAM\n");
  printf("-----------------------------------\n");
  printf("1. Register\n2. Activate\n3. Sign in\n4. Search\n5. Change password\n6. Sign out\n");
  printf("Your choice (1-6, other to quit): ");
  char c[100] = "";
  scanf("%[^\n]s", c);
  clearBuffer();

  return atoi(c);
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("No input file.\n");
    return -1;
  }

  Data d = createData("", "", IDLE, 4, 3);
  Node cur = createNode(d);
  Node head = setup(cur, argv[1]);

  while (1) {
    int choice = menu();

    if (choice == 1) {
      Register(head, cur, argv[1]);
    } else if (choice == 2) {
      Activate(head, cur, argv[1]);
    } else if (choice == 3) {
      SignIn(head, cur, argv[1]);
    } else if (choice == 4) {
      Search(head, cur);
    } else if (choice == 5) {
      ChangePassword(head, cur, argv[1]);
    } else if (choice == 6) {
      SignOut(head, cur, argv[1]);
    } else break;

  }
  destroy(head);
  return 0;
}