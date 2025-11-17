/*
 * Rule: FIO23-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO23-C violation
 */

#include <stdio.h>
 
int main(void) {
  printf("Hello, world!\n");
  if (fclose(stdout) == EOF) {
    /* Handle error */
  }
  return 0;
}