/*
 * Rule: PRE12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE12-C violation
 */

inline int Abs(int x) {
  return x < 0 ? -x : x;
}