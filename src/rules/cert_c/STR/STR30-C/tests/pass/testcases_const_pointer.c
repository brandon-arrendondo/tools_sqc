/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: Using const pointer prevents modification
 */

void func(void) {
    // Compliant: const pointer cannot be used to modify
    const char *str = "string literal";
    // str[0] = 'S'; would cause compiler error
}

int main(void) {
    func();
    return 0;
}
