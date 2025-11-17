/*
 * Rule: FIO20-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO20-C violation
 */

#include <stdio.h>

void func(void) {
  char* buf = NULL;
  size_t dummy = 0;
  if (getline(&buf, &dummy, stdin) == -1) {
	/* handle error */
  }
  printf("The user input %s\n", buf);
  free(buf);
}