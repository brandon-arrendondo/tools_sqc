/*
 * Rule: PRE00-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE00-C violation
 */

int operations = 0, calls_to_F = 0, calls_to_G = 0;
 
inline int f(int x) {
  ++operations;
  ++calls_to_F;
  return 2 * x;
}

inline int g(int x) {
  ++operations;
  ++calls_to_G;
  return x + 1;
}

 
void func(int x) {
  int y = f(x) + g(x);

}