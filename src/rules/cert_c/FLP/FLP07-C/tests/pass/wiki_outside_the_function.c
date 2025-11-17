/*
 * Rule: FLP07-C
 * Source: wiki
 * Status: PASS - Compliant solution
 */

void float_routine(void) {
  float value = 99.0f;
  long double percentage;

  percentage = (float) calc_percentage(value);
}