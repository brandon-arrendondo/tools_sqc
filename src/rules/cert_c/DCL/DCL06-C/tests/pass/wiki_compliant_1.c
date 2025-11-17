/*
 * Rule: DCL06-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL06-C violation
 */

enum { ADULT_AGE=18 };
/* ... */
if (age >= ADULT_AGE) {
   /* Take action */
}
else {
  /* Take a different action */
}
/* ... */