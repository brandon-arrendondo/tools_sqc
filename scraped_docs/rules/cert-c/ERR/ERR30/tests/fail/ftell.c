#include <errno.h>
#include <stdio.h>

void func(FILE* fp) { 
  errno=0;
  ftell(fp);
  if (errno) {
    perror("ftell");
  }
}