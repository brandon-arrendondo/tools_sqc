/*
 * Rule: EXP43-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP43-C violation
 */

void func(void) {
  int *restrict p1;   
  int *restrict q1;
  {  /* Added inner block */
    int *restrict p2 = p1; /* Valid, well-defined behavior */    
    int *restrict q2 = q1; /* Valid, well-defined behavior */ 
  }
}