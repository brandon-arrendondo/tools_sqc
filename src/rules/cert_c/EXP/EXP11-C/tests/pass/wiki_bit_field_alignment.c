/*
 * Rule: EXP11-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP11-C violation
 */

struct bf {
  unsigned int m1 : 8;
  unsigned int m2 : 8;
  unsigned int m3 : 8;
  unsigned int m4 : 8;
}; /* 32 bits total */

void function() {
  struct bf data;
  data.m1 = 0;
  data.m2 = 0;
  data.m3 = 0;
  data.m4 = 0;
  data.m1++;
}