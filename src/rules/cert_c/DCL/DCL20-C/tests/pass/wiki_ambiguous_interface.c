/*
 * Rule: DCL20-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL20-C violation
 */

/* In foo.h */
void foo(void);

/* In foo.c */
void foo(void) {
  int i = 3;
  printf("i value: %d\n", i);
}

/* In caller.c */
#include "foo.h"

foo(3);