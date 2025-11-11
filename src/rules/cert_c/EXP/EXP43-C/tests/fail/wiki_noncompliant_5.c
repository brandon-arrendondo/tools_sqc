/*
 * Rule: EXP43-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP43-C violation
 */

#include <stdio.h>
 
void func(void) {
  int i;
  float x;
  char format[100] = "%s";
  /* Undefined behavior */
  int n = scanf(format, format + 2, &i, &x); 
  /* ... */
}