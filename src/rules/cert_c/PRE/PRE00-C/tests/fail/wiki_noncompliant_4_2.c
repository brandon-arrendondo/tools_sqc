/*
 * Rule: PRE00-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE00-C violation
 */

#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void func(void) {
  int x = 5, y = 10;
  // VIOLATION: Function-like macro instead of inline function
  printf("%d\n", MAX(x, y));
}