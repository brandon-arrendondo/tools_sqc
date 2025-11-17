/*
 * Rule: DCL11-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL11-C violation
 */

long long a = 1;
const char msg[] = "Default message";
/* ... */
printf("%lld %s", a, msg);