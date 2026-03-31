/*
 * Rule: STR11-C
 * Source: testcases
 * Status: FAIL - Should trigger STR11-C violation
 * Description: Array bound too small for string literal plus null terminator
 */

const char greeting[5] = "hello";  /* Violation: needs 6 for '\0' */
const char msg[4] = "test";        /* Violation: needs 5 for '\0' */
char code[2] = "US";               /* Violation: needs 3 for '\0' */
