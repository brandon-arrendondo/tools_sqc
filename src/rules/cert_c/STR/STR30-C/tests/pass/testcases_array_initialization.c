/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: Using array initialization creates modifiable copy
 */

void func(void) {
    // Compliant: array initialization creates modifiable copy
    char str[] = "string literal";
    str[0] = 'S';  // Safe: modifying array, not literal
}

int main(void) {
    func();
    return 0;
}
