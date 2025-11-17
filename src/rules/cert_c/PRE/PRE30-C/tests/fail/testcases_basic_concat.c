/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Using ## to concatenate UCN parts (classic example from standard)
 */

#define assign(uc1, uc2, val) uc1##uc2 = val  // Line 7 - VIOLATION

void func(void) {
    int \u0401;

    // Concatenates \u04 and 01 to form \u0401
    assign(\u04, 01, 4);  // Line 13 - VIOLATION
}

int main(void) {
    func();
    return 0;
}
