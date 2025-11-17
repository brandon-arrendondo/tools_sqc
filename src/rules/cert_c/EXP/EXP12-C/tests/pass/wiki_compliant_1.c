/*
 * Rule: EXP12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP12-C violation
 */

void func(char* name) {
  char* s = NULL;
  if (asprintf(&s,"Hello, %s!\n", name) < 0) {
    /* Handle error */
  }
  (void) puts(s);
  free(s);
}