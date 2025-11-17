/*
 * Rule: FIO23-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO23-C violation
 */

#include <stdio.h>
 
void cleanup(void) {
  /* Do cleanup */

  printf("All cleaned up!\n");
  if (fflush(stdout) == EOF) {
    /* Handle error */
  }
}

int main(void) {
  atexit(cleanup);
  printf("Doing important stuff\n");

  /* Do important stuff */


  if (fflush(stdout) == EOF) {
    /* Handle error */
  }
  return 0;
}