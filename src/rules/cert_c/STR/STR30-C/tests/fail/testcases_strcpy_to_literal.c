/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Using strcpy() to modify string literal
 */

#include <string.h>

void copy_string(void) {
    char *dest = "destination";  // Line 10 - VIOLATION: non-const pointer to string literal
    strcpy(dest, "source");  // Line 11 - VIOLATION: modifying string literal
}

int main(void) {
    copy_string();
    return 0;
}
