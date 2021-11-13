#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}

typedef struct {
  int x, y;
} A;

int main() {
  A a[2];
  a[0].x = 5;
  a[0].y = 1;
  a[1].x = 2;
  a[1].y = 9;
  

  void* shmem = create_shared_memory(128);

  memcpy(shmem, a, sizeof(a));

  int pid = fork();

  if (pid == 0) {
    ((A*)shmem)[0].x = 100;

  } else {
    printf("Parent read: %d\n", ((A*)shmem)[0].x);
    sleep(1);
    printf("After 1s, parent read: %d\n", ((A*)shmem)[0].x);
  }
}