/*
 * Rule: PRE07-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE07-C violation
 */

size_t i = /* Some initial value */;
/* Assignment of i */
if (i > 9000) {
   if (puts("Over 9000!?""?!") == EOF) {
     /* Handle error */
   }
}