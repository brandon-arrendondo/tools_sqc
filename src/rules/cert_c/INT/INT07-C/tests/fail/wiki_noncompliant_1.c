/*
 * Rule: INT07-C
 * Source: wiki
 * Status: FAIL - Should trigger INT07-C violation
 */

char c = 200;
int i = 1000;
printf("i/c = %d\n", i/c);