/*
 * Rule: EXP16-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP16-C violation
 */

int do_xyz(void); 
 
int f(void) {
/* ... */ 
  if (do_xyz()) { 
    return -1; /* Indicate failure */
  }
/* ... */
  return 0;  
}