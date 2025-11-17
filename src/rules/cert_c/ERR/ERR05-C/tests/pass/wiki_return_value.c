/*
 * Rule: ERR05-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR05-C violation
 */

const errno_t ESOMETHINGREALLYBAD = 1;

errno_t g(void) {
  /* ... */
  if (something_really_bad_happens) {
    return ESOMETHINGREALLYBAD;
  }
  /* ... */
  return 0;
}

errno_t f(void) {
  errno_t status = g();
  if (status != 0) {
    return status;
  }

  /* ... Do the rest of f ... */

  return 0;
}