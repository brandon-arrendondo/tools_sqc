/*
 * Rule: EXP30-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP30-C violation
 */

#include <stdio.h>

void func(int i, int *b) {
  int a = i + b[++i];
  printf("%d, %d", a, i);
}