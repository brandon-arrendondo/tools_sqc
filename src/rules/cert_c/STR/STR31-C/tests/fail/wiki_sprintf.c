/*
 * Rule: STR31-C
 * Source: wiki
 * Status: FAIL - Should trigger STR31-C violation
 */

#include <stdio.h>
 
void func(const char *name) {
  char filename[128];
  sprintf(filename, "%s.txt", name);
}