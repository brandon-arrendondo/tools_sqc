/*
 * Rule: FIO46-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO46-C violation
 */

#include <stdio.h>
 
int close_stdout(void) {
  if (fclose(stdout) == EOF) {
    return -1;
  }

  fputs("stdout successfully closed.", stderr);
  return 0;
}