/*
 * Rule: INT05-C
 * Source: wiki
 * Status: FAIL - Should trigger INT05-C violation
 */

long num_long;

if (scanf("%ld", &num_long) != 1) {
  /* Handle error */
}