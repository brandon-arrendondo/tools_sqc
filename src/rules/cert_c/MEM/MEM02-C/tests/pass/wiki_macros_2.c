/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 */

widget *p;

/* ... */

p = MALLOC(widget);   /* OK */
if (p != NULL) {
  p->i = 0;           /* OK */
  p->d = 0.0;         /* OK */
}