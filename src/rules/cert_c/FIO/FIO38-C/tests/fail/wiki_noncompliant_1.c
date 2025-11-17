/*
 * Rule: FIO38-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO38-C violation
 */

#include <stdio.h>
 
int main(void) {
  FILE my_stdout = *stdout;
  if (fputs("Hello, World!\n", &my_stdout) == EOF) {
    /* Handle error */
  }
  return 0;
}