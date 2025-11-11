/*
 * Rule: EXP16-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP16-C violation
 */

int do_xyz(void); 
 
int f(void) {
/* ... */
  if (do_xyz) { 
    return -1; /* Indicate failure */ 
  }
/* ... */
  return 0;
}