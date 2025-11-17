/*
 * Rule: STR32-C
 * Source: wiki
 * Status: FAIL - Should trigger STR32-C violation
 */

#include <stdio.h>
 
void func(void) {
  char c_str[3] = "abc";
  printf("%s\n", c_str);
}