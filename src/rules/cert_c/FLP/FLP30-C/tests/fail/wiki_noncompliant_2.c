/*
 * Rule: FLP30-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP30-C violation
 */

void func(void) {
  for (float x = 100000001.0f; x <= 100000010.0f; x += 1.0f) {
    /* Loop may not terminate */
  }
}