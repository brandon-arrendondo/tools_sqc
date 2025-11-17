/*
 * Rule: FLP36-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP36-C violation
 */

#include <stdio.h>

int main(void) {
  long int big = 1234567890L;
  float approx = big;
  printf("%ld\n", (big - (long int)approx));
  return 0;
}