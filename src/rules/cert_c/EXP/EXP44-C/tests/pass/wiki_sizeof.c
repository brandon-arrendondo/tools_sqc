/*
 * Rule: EXP44-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP44-C violation
 */

#include <stdio.h>
 
void func(void) {
  int a = 14;
  int b = sizeof(a);
  ++a;
  printf("%d, %d\n", a, b);
}