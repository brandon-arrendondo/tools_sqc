/*
 * Rule: EXP02-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP02-C violation
 */

char *p = /* Initialize; may or may not be NULL */

if (p || (p = (char *) malloc(BUF_SIZE)) ) {
  /* Perform some computation based on p */
  free(p);
  p = NULL;
} else {
  /* Handle malloc() error */
  return;
}