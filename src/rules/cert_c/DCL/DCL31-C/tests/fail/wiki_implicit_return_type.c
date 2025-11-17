/*
 * Rule: DCL31-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL31-C violation
 */

#include <limits.h>
#include <stdio.h>
 
foo(void) {
  return UINT_MAX;
}

int main(void) {
  long long int c = foo();
  printf("%lld\n", c);
  return 0;
}