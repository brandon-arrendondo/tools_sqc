/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: Global const pointer prevents modification
 */

const char *global_str = "global string";  // Compliant: const pointer

void read_global(void) {
    // Can only read, not modify
    char first = global_str[0];
}

int main(void) {
    read_global();
    return 0;
}
