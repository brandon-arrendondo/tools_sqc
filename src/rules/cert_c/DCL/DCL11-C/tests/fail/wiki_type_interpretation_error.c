/*
 * Rule: DCL11-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL11-C violation
 */

const char *error_msg = "Error occurred";
/* ... */
printf("%s:%d", 15, error_msg);