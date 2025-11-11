/*
 * Rule: DCL20-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL20-C violation
 */

/* In foo.h */
void foo();

/* In foo.c */
void foo() {
  int i = 3;
  printf("i value: %d\n", i);
}

/* In caller.c */
#include "foo.h"

foo(3);