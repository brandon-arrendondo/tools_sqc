/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Using memset() to modify string literal
 */

#include <string.h>

void clear_string(void) {
    char *str = "sensitive data";  // Line 10 - VIOLATION: non-const pointer to string literal
    memset(str, 0, strlen(str));  // Line 11 - VIOLATION: modifying string literal
}

int main(void) {
    clear_string();
    return 0;
}
