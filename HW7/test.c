#include <stdio.h>

typedef struct {
  int x, y;
} A;

int main() {
  A a[2];
  a[0].x = 5;
  a[0].y = 1;
  a[1].x = 2;
  a[1].y = 9;

  int i;
  A *b;
  // for (i = 0; i < 2; i++) {
  //   b[i] = a[i];
  // }
  b = a;
  b[1].x = 100;

  for (i = 0; i < 2; i++) {
    printf("%d\t%d\n", a[i].x, a[i].y);
  }
  
  return 0;
}