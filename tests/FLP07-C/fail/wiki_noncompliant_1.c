// FLP07-C: Noncompliant - missing cast on return value
// Source: https://wiki.sei.cmu.edu/confluence/display/c/FLP07-C

float calc_percentage(float value) {
  return value * 0.1f;  // VIOLATION: No cast on return value
}

void float_routine(void) {
  float value = 99.0f;
  long double percentage;

  percentage = calc_percentage(value);
}
