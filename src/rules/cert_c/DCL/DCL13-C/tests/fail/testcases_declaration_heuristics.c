/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: FAIL - Function declarations with non-const readonly-looking params
 */

/* Parameter named "src" suggests read-only intent */
void process(char *dest, char *src);

/* Parameter named "input" suggests read-only intent */
int validate(char *input);
