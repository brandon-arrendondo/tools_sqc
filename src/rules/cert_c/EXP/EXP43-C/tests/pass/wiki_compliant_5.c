/*
 * Rule: EXP43-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP43-C violation
 */

#include <stdio.h>
 
void func(void) {
  int i;
  float x;
  int n = scanf("%d%f", &i, &x); /* Defined behavior  */ 
  /* ... */
}