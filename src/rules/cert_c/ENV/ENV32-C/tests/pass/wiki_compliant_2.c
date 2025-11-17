/*
 * Rule: ENV32-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ENV32-C violation
 */

#include <stdlib.h>

void exit1(void) {
  return;
}

int main(void) {
  if (atexit(exit1) != 0) {
    /* Handle error */
  }
  return 0;
}