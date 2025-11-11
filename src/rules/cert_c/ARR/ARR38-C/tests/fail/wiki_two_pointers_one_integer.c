/*
 * Rule: ARR38-C
 * Source: wiki
 * Status: FAIL - Should trigger ARR38-C violation
 */

#include <string.h>

void f4() {
  char p[40];
  const char *q = "Too short";
  size_t n = sizeof(p);
  memcpy(p, q, n);
}