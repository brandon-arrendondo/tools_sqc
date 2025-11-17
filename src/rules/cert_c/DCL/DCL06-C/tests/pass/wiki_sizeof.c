/*
 * Rule: DCL06-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL06-C violation
 */

char buffer[256];
/* ... */
fgets(buffer, sizeof(buffer), stdin);