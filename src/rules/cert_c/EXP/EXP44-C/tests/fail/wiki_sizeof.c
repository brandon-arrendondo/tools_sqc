/*
 * Rule: EXP44-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP44-C violation
 */

#include <stdio.h>
 
void func(void) {
  int a = 14;
  int b = sizeof(a++);
  printf("%d, %d\n", a, b);
}