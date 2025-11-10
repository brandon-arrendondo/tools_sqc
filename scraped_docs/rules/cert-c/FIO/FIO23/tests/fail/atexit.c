#include <stdio.h>
 
void cleanup(void) {
  /* Do cleanup */

  printf("All cleaned up!\n");
}

int main(void) {
  atexit(cleanup);
  printf("Doing important stuff\n");

  /* Do important stuff */

  if (fclose(stdout) == EOF) {
    /* Handle error */
  }
  return 0;
}