#include <stdio.h>
 
enum { BUFFERSIZE = 32 };

void func(void) {
  char buf[BUFFERSIZE];
  int ch;
  size_t index = 0;
  bool truncated = false;

  while ((ch = getchar()) != '\n' && ch != EOF) {
    if (index < sizeof(buf) - 1) {
      buf[index++] = (char)ch;
    } else {
      truncated = true;
    }
  }
  buf[index] = '\0';  /* Terminate string */
  if (ch == EOF) {
    /* Handle EOF or error */
  }
  if (truncated) {
    /* Handle truncation */
  }
}