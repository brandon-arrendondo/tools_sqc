// FLP07-C: Compliant - cast at call site
// Source: https://wiki.sei.cmu.edu/confluence/display/c/FLP07-C

float calc_percentage(float value);  // Declaration only

void float_routine(void) {
  float value = 99.0f;
  long double percentage;

  percentage = (float) calc_percentage(value);  // OK: Cast at call site
}
