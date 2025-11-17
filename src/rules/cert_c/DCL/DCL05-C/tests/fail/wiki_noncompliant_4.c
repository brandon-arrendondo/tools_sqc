/*
 * Rule: DCL05-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL05-C violation
 */

void (*signal(int, void (*)(int)))(int);