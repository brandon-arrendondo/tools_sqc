/*
 * Rule: STR31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR31-C violation
 */

#include <stdio.h>
 
void func(const char *name) {
  char filename[128];
  sprintf(filename, "%.*s.txt", sizeof(filename) - 5, name);
}