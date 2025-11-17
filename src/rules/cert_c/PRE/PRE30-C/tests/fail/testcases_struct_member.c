/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN for struct member access through concatenation
 */

struct data {
    int \u0440;  // Cyrillic small letter er
};

#define GET_MEMBER(s, m1, m2) s.m1##m2  // Line 11 - VIOLATION

void struct_test(void) {
    struct data d;

    // Creates \u0440 through concatenation
    GET_MEMBER(d, \u04, 40) = 100;  // Line 17 - VIOLATION
}

int main(void) {
    struct_test();
    return 0;
}
