#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"
#include "authenticate.h"

#include "connect.h"

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
    char **e = words(line, &total);
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
    
    Data d = createData(e[0], e[1], atoi(e[2]), e[3], activeTimes, signInTimes, isSignIn);
    head = addTail(head, d);
    free(e);
  }
  fclose(f);
  return head;
}

// ___________________________
void Register(Node head, char *fileName) {
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

  printf("Enter your website you like: ");
  char webAddress[100] = "";
  scanf("%[^\n]s", webAddress);
  clearBuffer();
  if (isIpV4(webAddress)) {
    if (!isIp(webAddress)) {
      strcpy(webAddress, "unknow");
    }
  } else {
    if (!isHostName(webAddress)) {
      strcpy(webAddress, "unknow");
    }
  };

  Data d = createData(username, password, IDLE, webAddress, 4, 3, 0);
  head = addTail(head, d);
  writeFile(head, fileName);
  success("Successful registration. Activation required.");
}

void Activate(Node head, char *fileName) {
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
        Data data = createData(temp.username, temp.password, BLOCKED, temp.address, 0, temp.signInTimes, 0);
        head = updateAt(head, userIndex, data);
        writeFile(head, fileName);
        err("Activation code is incorrect. Account is blocked");
        return;
      }

      Data newData = createData(temp.username, temp.password, IDLE, temp.address, --temp.activeTimes, temp.signInTimes, temp.isSignIn);
      head = updateAt(head, userIndex, newData);
      writeFile(head, fileName);
      err("Account is not activated");
      goto enterCode;
    } else {
      Data newData = createData(temp.username, temp.password, ACTIVE, temp.address, 4, temp.signInTimes, temp.isSignIn);
      head = updateAt(head, userIndex, newData);
      writeFile(head, fileName);
      success("Account is activated");
    }
  }
}

void SignIn(Node head, char *fileName) {
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

  if (isUserSignIn(head, username)) {
    char s[100] = "Hello ";
    strcat(s, username);
    success(s);
    return;
  }

  signIn: {
    printf("Enter your password: ");
    scanf("%[^\n]s", password);
    clearBuffer();

    if (strcmp(temp.password, password) != 0) {
      if (temp.signInTimes <= 0) {
        Data data = createData(temp.username, temp.password, BLOCKED, temp.address, temp.activeTimes, 0, 0);
        head = updateAt(head, userIndex, data);
        writeFile(head, fileName);
        err("Password is incorrect. Account is blocked");
        return;
      }

      Data d = createData(temp.username, temp.password, temp.status, temp.address, temp.activeTimes, --temp.signInTimes, temp.isSignIn);
      head = updateAt(head, userIndex, d);
      writeFile(head, fileName);
      err("Password is incorrect");
      goto signIn;
    } else {
      Data d = createData(temp.username, temp.password, temp.status, temp.address, temp.activeTimes, 3, 1);
      head = updateAt(head, userIndex, d);
      writeFile(head, fileName);
      char s[100] = "Hello ";
      strcat(s, temp.username);
      success(s);
    }
  }
}

void Search(Node head) {
  if (!hasUserSignIn(head)) {
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

  char STATUS[3][10] = {"Blocked","Active", "Idle"};
  char s[50] = "Account is ";
  strcat(s, STATUS[d.status]);
  success(s);
}

void ChangePassword(Node head, char *fileName) {
  char username[100], password[100], newPassword[100];
  printf("Enter your username: ");
  scanf("%[^\n]s", username);
  clearBuffer();

  int userIndex = searchName(head, username);
  if (userIndex == -1) {
    err("Cannot find account");
    return;
  }

  if (!isUserSignIn(head, username)) {
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

  Data newData = createData(d.username, newPassword, d.status, d.address, d.activeTimes, d.signInTimes, d.isSignIn);
  head = updateAt(head, userIndex, newData);
  writeFile(head, fileName);

}

void SignOut(Node head, char *fileName) {

  char username[100];
  printf("Enter your username: ");
  scanf("%[^\n]s", username);
  clearBuffer();

  int userIndex = searchName(head, username);
  if (userIndex == -1) {
    err("Cannot find account");
    return;
  }

  if (!isUserSignIn(head, username)) {
    err("Account is not sign in");
    return;
  }
  Data d = get(head, userIndex);
  Data newData = createData(d.username, d.password, d.status, d.address, d.activeTimes, d.signInTimes, 0);
  head = updateAt(head, userIndex, newData);
  writeFile(head, fileName);
  char s[100] = "Goodbye ";
  strcat(s, username);
  success(s);
}

void HomePageWithDomainName(Node head) {
  char username[100];
  printf("Enter your username: ");
  scanf("%[^\n]s", username);
  clearBuffer();

  int userIndex = searchName(head, username);
  if (userIndex == -1) {
    err("Cannot find account");
    return;
  }

  if (!isUserSignIn(head, username)) {
    err("Account is not sign in");
    return;
  }

  Data d = get(head, userIndex);

  if (isIpV4(d.address)) {
    if (isIp(d.address)) {
      char hostname[100];
      get_hostName(d.address, hostname);
      primary(hostname);
      return;
    };
  } else {
    if (isHostName(d.address)) {
      primary(d.address);
    return;
    }
  }
  err("unknow");
}

void HomePageWithIPAddress(Node head) {
  char username[100];
  printf("Enter your username: ");
  scanf("%[^\n]s", username);
  clearBuffer();

  int userIndex = searchName(head, username);
  if (userIndex == -1) {
    err("Cannot find account");
    return;
  }

  if (!isUserSignIn(head, username)) {
    err("Account is not sign in");
    return;
  }

  Data d = get(head, userIndex);

  if (isIpV4(d.address)) {
    if (isIp(d.address)) {
      primary(d.address);
      return;
    }
  } else {
    if (isHostName(d.address)) {
      char ip[100];
      get_ip(d.address, ip);
      primary(ip);
      return;
    }
  }
  err("unknow");
}

int menu() {
  printf("USER MANAGEMENT PROGRAM\n");
  printf("-----------------------------------\n");
  printf("1. Register\n2. Activate\n3. Sign in\n4. Search\n5. Change password\n6. Sign out\n7. Homepage with domain name\n8. Homepage with IP address\n");
  printf("Your choice (1-8, other to quit): ");
  char c[100] = "";
  scanf("%[^\n]s", c);
  clearBuffer();

  return atoi(c);
}

