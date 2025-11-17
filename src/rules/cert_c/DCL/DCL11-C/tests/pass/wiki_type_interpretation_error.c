/*
 * Rule: DCL11-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL11-C violation
 */

const char *error_msg = "Error occurred";
/* ... */
printf("%d:%s", 15, error_msg);