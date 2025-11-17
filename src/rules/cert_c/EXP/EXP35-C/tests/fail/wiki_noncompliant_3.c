/*
 * Rule: EXP35-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP35-C violation
 */

#include <stdio.h>

struct X { int a[6]; };

struct X addressee(void) {
  struct X result = { { 1, 2, 3, 4, 5, 6 } };
  return result;
}

int main(void) {
  int *my_a = addressee().a;
  printf("%x", my_a[0]);
  return 0;
}