/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: Struct member is const char*, prevents modification
 */

struct data {
    const char *name;
};

void func(void) {
    // Compliant: const member prevents modification
    struct data d = { "literal" };
    // d.name[0] = 'L'; would cause compiler error
    char first = d.name[0];  // Only reading allowed
}

int main(void) {
    func();
    return 0;
}
