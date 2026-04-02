/*
 * Rule: ERR07-C
 * Status: FAIL - atol lacks error checking, prefer strtol
 */

long atol(const char *str);

void f(const char *str) {
    long val = atol(str);  /* VIOLATION: use strtol instead */
}
