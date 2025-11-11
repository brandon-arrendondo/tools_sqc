/*
 * Rule: FLP03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FLP03-C violation
 */

#include <fenv.h>
#pragma STDC FENV_ACCESS ON

void fpOper_fenv(void) {
  double a = 1e-40, b, c = 0.1;
  float x = 0, y;
  int fpeRaised;
  /* ... */

  feclearexcept(FE_ALL_EXCEPT);
  /* Store a into y is inexact and underflows: */
  y = a;
  fpeRaised = fetestexcept(FE_ALL_EXCEPT);
  /* fpeRaised has FE_INEXACT and FE_UNDERFLOW */

  feclearexcept(FE_ALL_EXCEPT);

  /* Divide-by-zero operation */
  b = y / x;
  fpeRaised = fetestexcept(FE_ALL_EXCEPT);
  /* fpeRaised has FE_DIVBYZERO */

  feclearexcept(FE_ALL_EXCEPT);

  c = sin(30) * a;
  fpeRaised = fetestexcept(FE_ALL_EXCEPT);
  /* fpeRaised has FE_INEXACT */

  feclearexcept(FE_ALL_EXCEPT);
  /* ... */
}