/*
 * Rule: DCL40-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL40-C violation
 */

/* In a.c */
extern int a[];   

int f(unsigned int i, int x) {
  int tmp = a[i];   
  a[i] = x;         
  return tmp;
}

/* In b.c */
int a[] = { 1, 2, 3, 4 };