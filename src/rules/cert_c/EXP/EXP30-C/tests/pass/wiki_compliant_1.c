/*
 * Rule: EXP30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP30-C violation
 */

#include <stdio.h>

void func(int i, int *b) {
  int a;
  ++i;
  a = i + b[i];
  printf("%d, %d", a, i);
}