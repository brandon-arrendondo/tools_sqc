/*
 * Rule: FLP07-C
 * Source: wiki
 * Status: PASS - Compliant solution - cast at call site
 */

float calc_percentage(float value) {
  return value * 0.1f;
}

void float_routine(void) {
  float value = 99.0f;
  long double percentage;

  percentage = (long double) calc_percentage(value);  // Explicit cast at call site
}