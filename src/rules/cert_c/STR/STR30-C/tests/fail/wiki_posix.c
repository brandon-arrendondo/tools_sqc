/*
 * Rule: STR30-C
 * Source: wiki
 * Status: FAIL - Should trigger STR30-C violation
 */

#include <stdlib.h>
 
void func(void) {
  mkstemp("/tmp/edXXXXXX");
}