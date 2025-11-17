/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying string literal from array of pointers
 */

void func(void) {
    char *strings[] = { "first", "second", "third" };  // Line 8 - VIOLATION: non-const pointers
    strings[1][0] = 'S';  // Line 9 - VIOLATION: modifying string literal
}

int main(void) {
    func();
    return 0;
}
