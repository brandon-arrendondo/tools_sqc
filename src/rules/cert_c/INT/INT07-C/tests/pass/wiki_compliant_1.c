/*
 * Rule: INT07-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT07-C violation
 */

unsigned char c = 200;
int i = 1000;
printf("i/c = %d\n", i/c);