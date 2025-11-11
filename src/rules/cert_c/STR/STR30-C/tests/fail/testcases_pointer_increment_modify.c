/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying string literal through incremented pointer
 */

void uppercase_string(void) {
    char *str = "hello";  // Line 8 - VIOLATION: non-const pointer to string literal
    while (*str) {
        *str = *str - 32;  // Line 10 - VIOLATION: modifying string literal
        str++;
    }
}

int main(void) {
    uppercase_string();
    return 0;
}
