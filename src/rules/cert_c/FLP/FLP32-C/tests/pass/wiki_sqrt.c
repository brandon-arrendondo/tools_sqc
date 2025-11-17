/*
 * Rule: FLP32-C
 * Source: wiki
 * Status: PASS - Compliant solution
 */

#include <math.h>
 
void func(double x) {
  double result;

  if (isless(x, 0.0)) {
    /* Handle domain error */
  }

  result = sqrt(x);
}