/*
 * Rule: FLP32-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FLP32-C violation
 */

#include <math.h>
 
void func(double x) {
  double result;

  if (isless(x, 0.0)) {
    /* Handle domain error */
  }

  result = sqrt(x);
}