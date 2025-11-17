/*
 * Rule: DCL10-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL10-C violation
 */

const char *error_msg = "Resource not available to user.";
/* ... */
printf("Error: %s", error_msg);