/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Using sprintf() to write to string literal
 */

#include <stdio.h>

void format_string(void) {
    char *buffer = "formatted output";  // Line 10 - VIOLATION: non-const pointer to string literal
    sprintf(buffer, "Value: %d", 42);  // Line 11 - VIOLATION: modifying string literal
}

int main(void) {
    format_string();
    return 0;
}
