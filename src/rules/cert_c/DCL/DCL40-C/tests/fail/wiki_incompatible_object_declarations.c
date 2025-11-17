/*
 * Rule: DCL40-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL40-C violation
 */

/* In a.c */
extern int i;   /* UB 14 */

int f(void) {
  return ++i;   /* UB 36 */
}

/* In b.c */
short i;   /* UB 14 */