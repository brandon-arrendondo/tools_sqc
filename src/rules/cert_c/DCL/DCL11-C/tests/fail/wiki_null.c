/*
 * Rule: DCL11-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL11-C violation
 */

char* string = NULL;
printf("%s %d\n", string, 1);