/*
 * Rule: ERR05-C
 * Source: wiki
 * Status: FAIL - Should trigger ERR05-C violation
 */

void g(void) {
  /* ... */
  if (something_really_bad_happens) {
    fprintf(stderr, "Something really bad happened!\n");
    abort();
  }
  /* ... */
}

void f(void) {
  g();
  /* ... Do the rest of f ... */
}