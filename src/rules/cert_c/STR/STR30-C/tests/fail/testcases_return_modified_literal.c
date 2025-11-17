/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Returning and modifying string literal
 */

char *get_string(void) {
    return "returned literal";  // Line 8 - VIOLATION: returning non-const pointer
}

void func(void) {
    char *str = get_string();
    str[0] = 'R';  // Line 13 - VIOLATION: modifying string literal
}

int main(void) {
    func();
    return 0;
}
