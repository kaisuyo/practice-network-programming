#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

char a[] = "hello";

void sighandler(int flag) {
  // printf("%d", n);
  puts(a);
  exit(0);
}

int main()
{
  signal(SIGINT, (void (*)(int))sighandler);
  while (1)
  {
    printf("hello world\n");
    sleep(1);
  }
  return 0;
}