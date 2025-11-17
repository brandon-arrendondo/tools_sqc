/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN in bitwise operation through concatenation
 */

#define BITWISE_OR(v1, v2, val) (v1##v2 | val)  // Line 7 - VIOLATION

void bitwise_test(void) {
    int \u0550 = 0x0F;  // Armenian capital letter cha

    // Creates \u0550 through concatenation
    int result = BITWISE_OR(\u05, 50, 0xF0);  // Line 13 - VIOLATION
}

int main(void) {
    bitwise_test();
    return 0;
}
