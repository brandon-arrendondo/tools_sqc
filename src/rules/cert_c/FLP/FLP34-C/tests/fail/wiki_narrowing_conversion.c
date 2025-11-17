/*
 * Rule: FLP34-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP34-C violation
 */

void func(double d_a, long double big_d) {
  double d_b = (float)big_d;
  float f_a = (float)d_a;
  float f_b = (float)big_d;
}