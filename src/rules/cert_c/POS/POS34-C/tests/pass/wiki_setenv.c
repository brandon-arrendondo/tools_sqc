/*
 * Rule: POS34-C
 * Source: wiki
 * Status: PASS - Should NOT trigger POS34-C violation
 */

int func(const char *var) {
  return setenv("TEST", var, 1);
}