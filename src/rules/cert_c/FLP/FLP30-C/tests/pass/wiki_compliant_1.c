/*
 * Rule: FLP30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FLP30-C violation
 */

#include <stddef.h>
 
void func(void) {
  for (size_t count = 1; count <= 10; ++count) {
    float x = count / 10.0f;
    /* Loop iterates exactly 10 times */
  }
}