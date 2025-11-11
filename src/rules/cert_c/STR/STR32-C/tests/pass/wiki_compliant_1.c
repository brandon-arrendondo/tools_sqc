/*
 * Rule: STR32-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR32-C violation
 */

#include <stdio.h>
 
void func(void) {
  char c_str[] = "abc";
  printf("%s\n", c_str);
}