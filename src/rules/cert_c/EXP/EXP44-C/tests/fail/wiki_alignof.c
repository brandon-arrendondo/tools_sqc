/*
 * Rule: EXP44-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP44-C violation
 */

#include <stdio.h>
 
void func(void) {
  int val = 0; 
  /* ... */ 
  size_t align = _Alignof(int[++val]);
  printf("%zu, %d\n", align, val);
  /* ... */
}