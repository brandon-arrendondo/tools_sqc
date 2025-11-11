/*
 * Rule: EXP43-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP43-C violation
 */

int *restrict a;
int *restrict b;

extern int c[];
 
int main(void) {
  c[0] = 17; 
  c[1] = 18;
  a = &c[0]; 
  b = &c[1];
  a = b; /* Undefined behavior */
  /* ... */
}