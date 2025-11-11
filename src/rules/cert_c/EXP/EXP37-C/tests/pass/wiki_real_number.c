/*
 * Rule: EXP37-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP37-C violation
 */

#include <tgmath.h>
 
void func(void) {
  double complex c = 2.0 + 4.0 * I;
  double complex result = log2(creal(c));
}