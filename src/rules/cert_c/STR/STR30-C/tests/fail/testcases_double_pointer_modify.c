/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying string literal through double pointer
 */

void func(void) {
    char *str = "literal";  // Line 8 - VIOLATION: non-const pointer to string literal
    char **ptr = &str;
    (*ptr)[0] = 'L';  // Line 10 - VIOLATION: modifying string literal
}

int main(void) {
    func();
    return 0;
}
