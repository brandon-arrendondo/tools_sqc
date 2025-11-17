/*
 * Rule: DCL40-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL40-C violation
 */

/* In a.c */
extern int i;   

int f(void) {
  return ++i;   
}

/* In b.c */
int i;