/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: PASS
 * Reason: Passing complete UCN as macro argument without concatenation
 */

#define assign(ucn, val) ucn = val  // No concatenation - COMPLIANT

void func(void) {
    int \u0401;

    // Complete UCN passed as argument - COMPLIANT
    assign(\u0401, 4);
}

int main(void) {
    func();
    return 0;
}
