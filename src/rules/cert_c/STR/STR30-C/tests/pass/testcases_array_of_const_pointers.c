/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: Array of const pointers prevents modification
 */

void func(void) {
    // Compliant: const pointers prevent modification
    const char *strings[] = { "first", "second", "third" };
    // strings[1][0] = 'S'; would cause compiler error

    // Can only read
    char c = strings[1][0];
}

int main(void) {
    func();
    return 0;
}
