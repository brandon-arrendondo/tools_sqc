/*
 * Rule: FLP32-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP32-C violation
 */

#include <math.h>
 
void func(float x) {
  float result = asin(x);
  /* ... */
}