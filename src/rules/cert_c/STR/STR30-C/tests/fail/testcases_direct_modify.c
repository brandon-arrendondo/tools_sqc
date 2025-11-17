/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Directly modifying a string literal via pointer
 */

void func(void) {
    char *str = "string literal";  // Line 8 - VIOLATION: non-const pointer to string literal
    str[0] = 'S';  // Line 9 - VIOLATION: modifying string literal
}

int main(void) {
    func();
    return 0;
}
