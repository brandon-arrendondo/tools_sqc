/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Using strncpy() to write to string literal
 */

#include <string.h>

void copy_n_chars(void) {
    char *dest = "destination";  // Line 10 - VIOLATION: non-const pointer to string literal
    strncpy(dest, "source", 5);  // Line 11 - VIOLATION: modifying string literal
}

int main(void) {
    copy_n_chars();
    return 0;
}
