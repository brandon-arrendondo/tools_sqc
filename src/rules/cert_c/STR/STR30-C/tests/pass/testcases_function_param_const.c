/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: Function parameter is const, prevents modification
 */

void read_string(const char *s) {
    // Compliant: const parameter, cannot modify
    // s[0] = 'X'; would cause compiler error
    char c = s[0];  // Only reading is allowed
}

void caller(void) {
    read_string("test");  // Safe: function cannot modify literal
}

int main(void) {
    caller();
    return 0;
}
