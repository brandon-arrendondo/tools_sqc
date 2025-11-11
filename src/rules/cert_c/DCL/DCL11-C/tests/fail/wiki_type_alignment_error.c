/*
 * Rule: DCL11-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL11-C violation
 */

long long a = 1;
const char msg[] = "Default message";
/* ... */
printf("%d %s", a, msg);