/*
 * Rule: EXP36-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP36-C violation
 */

#include <assert.h>
 
void func(void) {
  char c = 'x';
  int i = c;
  int *ip = &i;

  assert(ip == &i);
}