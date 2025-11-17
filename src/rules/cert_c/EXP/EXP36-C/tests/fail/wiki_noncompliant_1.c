/*
 * Rule: EXP36-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP36-C violation
 */

#include <assert.h>
 
void func(void) {
  char c = 'x';
  int *ip = (int *)&c; /* This can lose information */
  char *cp = (char *)ip;

  /* Will fail on some conforming implementations */
  assert(cp == &c);
}