/*
 * Rule: EXP37-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP37-C violation
 */

#include <stdio.h>
#include <string.h>

char *(*fp)(const char *, int);

int main(void) {
  const char *c;
  fp = strchr;
  c = fp("Hello",'e');
  printf("%s\n", c);
  return 0;
}