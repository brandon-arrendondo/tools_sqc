/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Global pointer to string literal, later modified
 */

char *global_str = "global string";  // Line 7 - VIOLATION: non-const pointer to string literal

void modify_global(void) {
    global_str[0] = 'G';  // Line 10 - VIOLATION: modifying string literal
}

int main(void) {
    modify_global();
    return 0;
}
