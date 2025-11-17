/*
 * Rule: EXP11-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP11-C violation
 */

struct bf {
  unsigned int m1 : 8;
  unsigned int m2 : 8;
  unsigned int m3 : 8;
  unsigned int m4 : 8;
};	/* 32 bits total */