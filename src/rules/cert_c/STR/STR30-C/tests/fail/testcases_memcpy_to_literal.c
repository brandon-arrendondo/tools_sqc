/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Using memcpy() to write to string literal
 */

#include <string.h>

void copy_to_literal(void) {
    char *dest = "destination";  // Line 10 - VIOLATION: non-const pointer to string literal
    char src[] = "source";
    memcpy(dest, src, strlen(src));  // Line 12 - VIOLATION: modifying string literal
}

int main(void) {
    copy_to_literal();
    return 0;
}
