/*
 * Rule: EXP36-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP36-C violation
 */

int *loop_function(int *v_pointer) {
  /* ... */
  return v_pointer;
}
 
void func(int *loop_ptr) {
  int *int_ptr = loop_function(loop_ptr);

  /* ... */
}