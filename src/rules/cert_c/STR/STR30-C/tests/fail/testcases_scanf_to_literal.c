/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Using scanf() to read into string literal
 */

#include <stdio.h>

void read_input(void) {
    char *buffer = "input buffer";  // Line 10 - VIOLATION: non-const pointer to string literal
    scanf("%s", buffer);  // Line 11 - VIOLATION: modifying string literal
}

int main(void) {
    return 0;
}
