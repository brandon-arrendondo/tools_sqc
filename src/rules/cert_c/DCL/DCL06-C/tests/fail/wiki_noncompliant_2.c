/*
 * Rule: DCL06-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL06-C violation
 */

char buffer[256];
/* ... */
fgets(buffer, 256, stdin);