/*
 * Rule: PRE13-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE13-C violation
 */

#include <stdio.h>

int main(void) {
  #if (__STDC__ == 1)
    printf("Implementation is ISO-conforming.\n");
  #else
    printf("Implementation is not ISO-conforming.\n");
  #endif
  /* ... */

  return 0;
}