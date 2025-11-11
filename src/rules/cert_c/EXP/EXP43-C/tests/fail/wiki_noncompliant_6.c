/*
 * Rule: EXP43-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP43-C violation
 */

void func(void) {
  int *restrict p1;
  int *restrict q1;

  int *restrict p2 = p1; /* Undefined behavior */ 
  int *restrict q2 = q1; /* Undefined behavior */ 
 }