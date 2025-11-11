/*
 * Rule: FLP32-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP32-C violation
 */

#include <math.h>
 
void func(double x) {
  double result;
  result = sinh(x);
}