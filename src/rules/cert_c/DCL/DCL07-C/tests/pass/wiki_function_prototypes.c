/*
 * Rule: DCL07-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL07-C violation
 */

/* file_b.c source file */
int func(int, int, int);

func(1, 2, 3);