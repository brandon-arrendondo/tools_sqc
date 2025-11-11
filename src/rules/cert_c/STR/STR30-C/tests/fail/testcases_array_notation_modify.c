/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying string literal using array notation
 */

void swap_chars(void) {
    char *str = "abcdef";  // Line 8 - VIOLATION: non-const pointer to string literal
    char temp = str[0];
    str[0] = str[5];  // Line 10 - VIOLATION: modifying string literal
    str[5] = temp;  // Line 11 - VIOLATION: modifying string literal
}

int main(void) {
    swap_chars();
    return 0;
}
