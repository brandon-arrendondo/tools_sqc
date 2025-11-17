/*
 * Rule: EXP40-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP40-C violation
 */

int **ipp;
int *ip;
int i = 42;

void func(void) {
  ipp = &ip; /* Valid */
  *ipp = &i; /* Valid */
  *ip = 0; /* Valid */
}