/*
 * Rule: FLP30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FLP30-C violation
 */

void func(void) {
  for (size_t count = 1; count <= 10; ++count) {
    float x = 100000000.0f + (count * 1.0f);
    /* Loop iterates exactly 10 times */
  }
}