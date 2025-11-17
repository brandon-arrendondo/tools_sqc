/*
 * Rule: PRE31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

#include <assert.h>
#include <stddef.h>
  
void process(size_t index) {
  assert(index > 0); /* No side effect */
  ++index;
  /* ... */
}