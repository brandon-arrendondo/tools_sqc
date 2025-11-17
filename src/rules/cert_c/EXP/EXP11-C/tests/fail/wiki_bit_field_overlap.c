/*
 * Rule: EXP11-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP11-C violation
 */

struct bf {
  unsigned int m1 : 6;
  unsigned int m2 : 4;
};

void function() {
  unsigned char *ptr;
  struct bf data;
  data.m1 = 0;
  data.m2 = 0;
  ptr = (unsigned char *)&data;
  ptr++;
  *ptr += 1; /* What does this increment? */
}