/*
 * Rule: DCL10-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL10-C violation
 */

const char *error_msg = "Resource not available to user.";
/* ... */
printf("Error (%s): %s", error_msg);