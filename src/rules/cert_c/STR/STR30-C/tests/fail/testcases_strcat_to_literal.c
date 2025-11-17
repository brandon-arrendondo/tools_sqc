/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Using strcat() to modify string literal
 */

#include <string.h>

void append_string(void) {
    char *str = "Hello";  // Line 10 - VIOLATION: non-const pointer to string literal
    strcat(str, " World");  // Line 11 - VIOLATION: modifying string literal
}

int main(void) {
    append_string();
    return 0;
}
