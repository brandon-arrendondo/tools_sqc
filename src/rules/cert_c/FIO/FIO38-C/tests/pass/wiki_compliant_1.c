/*
 * Rule: FIO38-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO38-C violation
 */

#include <stdio.h>
 
int main(void) {
  FILE *my_stdout = stdout;
  if (fputs("Hello, World!\n", my_stdout) == EOF) {
    /* Handle error */
  }
  return 0;
}