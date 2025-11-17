/*
 * Rule: FIO46-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO46-C violation
 */

#include <stdio.h>
 
int close_stdout(void) {
  if (fclose(stdout) == EOF) {
    return -1;
  }
 
  printf("stdout successfully closed.\n");
  return 0;
}