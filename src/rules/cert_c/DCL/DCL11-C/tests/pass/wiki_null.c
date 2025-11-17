/*
 * Rule: DCL11-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL11-C violation
 */

char* string = NULL;
printf("%s %d\n", (string ? string : "null"), 1);