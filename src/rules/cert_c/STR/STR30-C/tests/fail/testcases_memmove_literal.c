/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Using memmove() to modify string literal
 */

#include <string.h>

void shift_string(void) {
    char *str = "shift me";  // Line 10 - VIOLATION: non-const pointer to string literal
    memmove(str, str + 1, strlen(str));  // Line 11 - VIOLATION: modifying string literal
}

int main(void) {
    shift_string();
    return 0;
}
