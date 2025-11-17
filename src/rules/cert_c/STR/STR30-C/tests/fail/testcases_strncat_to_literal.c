/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Using strncat() to append to string literal
 */

#include <string.h>

void append_n_chars(void) {
    char *str = "Hello";  // Line 10 - VIOLATION: non-const pointer to string literal
    strncat(str, " World", 6);  // Line 11 - VIOLATION: modifying string literal
}

int main(void) {
    append_n_chars();
    return 0;
}
