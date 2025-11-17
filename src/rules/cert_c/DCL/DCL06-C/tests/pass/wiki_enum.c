/*
 * Rule: DCL06-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL06-C violation
 */

enum { BUFFER_SIZE=256 };

char buffer[BUFFER_SIZE];
/* ... */
fgets(buffer, BUFFER_SIZE, stdin);