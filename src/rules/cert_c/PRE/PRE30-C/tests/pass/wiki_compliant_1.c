/*
 * Rule: PRE30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

#define assign(ucn, val) ucn = val
 
void func(void) {
  int \u0401;
  /* ... */
  assign(\u0401, 4);
  /* ... */
}