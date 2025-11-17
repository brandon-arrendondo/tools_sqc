/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Casting away const qualifier and modifying string literal
 */

void func(void) {
    const char *cstr = "constant string";
    char *str = (char *)cstr;  // Line 9 - VIOLATION: casting away const
    str[0] = 'C';  // Line 10 - VIOLATION: modifying string literal
}

int main(void) {
    func();
    return 0;
}
