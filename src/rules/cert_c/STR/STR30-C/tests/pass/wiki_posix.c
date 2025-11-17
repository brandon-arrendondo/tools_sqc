/*
 * Rule: STR30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR30-C violation
 */

#include <stdlib.h>
 
void func(void) {
  static char fname[] = "/tmp/edXXXXXX";
  mkstemp(fname);
}