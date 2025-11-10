#include <stdio.h>
 
int main(void) {
  printf("Hello, world!\n");
  if (fclose(stdout) == EOF) {
    /* Handle error */
  }
  return 0;
}