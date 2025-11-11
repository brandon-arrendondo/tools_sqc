/*
 * Rule: EXP35-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP35-C violation
 */

#include <stdio.h>

struct X { char a[8]; };

struct X salutation(void) {
  struct X result = { "Hello" };
  return result;
}

struct X addressee(void) {
  struct X result = { "world" };
  return result;
}

int main(void) {
  printf("%s, %s!\n", salutation().a, addressee().a);
  return 0;
}