/*
 * Rule: EXP32-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP32-C violation
 */

#include <stdio.h>

void func(void) {
  static volatile int **ipp;
  static volatile int *ip;
  static volatile int i = 0;

  printf("i = %d.\n", i);

  ipp = &ip;
  *ipp = &i;
  if (*ip != 0) {
    /* ... */
  }

}