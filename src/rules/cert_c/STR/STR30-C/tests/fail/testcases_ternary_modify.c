/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying string literal selected by ternary operator
 */

void func(int condition) {
    char *str = condition ? "option1" : "option2";  // Line 8 - VIOLATION: non-const pointer
    str[0] = 'O';  // Line 9 - VIOLATION: modifying string literal
}

int main(void) {
    func(1);
    return 0;
}
