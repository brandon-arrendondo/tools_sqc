/*
 * Rule: FLP07-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FLP07-C violation
 */

void float_routine(void) {
  float value = 99.0f;
  long double percentage;

  percentage = (float) calc_percentage(value);
}