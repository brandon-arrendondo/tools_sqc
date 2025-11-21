// FLP07-C: Compliant - cast at assignment
// Source: https://wiki.sei.cmu.edu/confluence/display/c/FLP07-C

float calc_percentage(float value) {
  return value * 0.1f;
}

void float_routine(void) {
  float value = 99.0f;
  long double percentage;
  
  percentage = (long double) calc_percentage(value);  // OK: Explicit cast at call site
}
