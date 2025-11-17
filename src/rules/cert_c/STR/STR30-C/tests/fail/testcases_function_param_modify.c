/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Function modifies parameter, string literal passed
 */

void modify_string(char *s) {
    s[0] = 'X';  // Line 8 - VIOLATION: modifying string literal
}

void caller(void) {
    modify_string("test");  // Line 12 - VIOLATION: passing string literal to modifying function
}

int main(void) {
    caller();
    return 0;
}
