/*
 * Rule: EXP02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP02-C violation
 */

char *p = /* Initialize; may or may not be NULL */
char *q = NULL;
if (p == NULL) {
  q = (char *) malloc(BUF_SIZE);
  p = q;
}
if (p == NULL) {
  /* Handle malloc() error */
  return;
}

/* Perform some computation based on p */
free(q);
q = NULL;