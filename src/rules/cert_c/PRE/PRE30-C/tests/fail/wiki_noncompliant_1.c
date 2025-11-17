/*
 * Rule: PRE30-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE30-C violation
 */

#define assign(uc1, uc2, val) uc1##uc2 = val

void func(void) {
  int \u0401;
  /* ... */
  assign(\u04, 01, 4);
  /* ... */
}