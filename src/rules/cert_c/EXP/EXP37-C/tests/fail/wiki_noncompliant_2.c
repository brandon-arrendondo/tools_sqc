/*
 * Rule: EXP37-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP37-C violation
 */

#include <stdio.h>
#include <string.h>

char *(*fp)();

int main(void) {
  const char *c;
  fp = strchr;
  c = fp('e', "Hello");
  printf("%s\n", c);
  return 0;
}