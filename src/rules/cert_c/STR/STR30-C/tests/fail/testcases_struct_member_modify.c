/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying string literal stored in struct member
 */

struct data {
    char *name;
};

void func(void) {
    struct data d = { "literal" };  // Line 12 - VIOLATION: non-const pointer to string literal
    d.name[0] = 'L';  // Line 13 - VIOLATION: modifying string literal
}

int main(void) {
    func();
    return 0;
}
