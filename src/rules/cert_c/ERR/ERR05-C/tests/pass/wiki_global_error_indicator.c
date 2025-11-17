/*
 * Rule: ERR05-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR05-C violation
 */

errno_t my_errno; /* Also declared in a .h file */
const errno_t ESOMETHINGREALLYBAD = 1;

void g(void) {
  /* ... */
  if (something_really_bad_happens) {
    my_errno = ESOMETHINGREALLYBAD;
    return;
  }
  /* ... */
}

void f(void) {
  my_errno = 0;
  g();
  if (my_errno != 0) {
    return;
  }
  /* ... Do the rest of f ... */
}