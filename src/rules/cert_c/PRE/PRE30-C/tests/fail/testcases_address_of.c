/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN for address-of operation through concatenation
 */

#define GET_ADDR(v1, v2) &(v1##v2)  // Line 7 - VIOLATION

void address_test(void) {
    int \u04A0 = 25;  // Cyrillic capital letter bashkir ka

    // Creates \u04A0 through concatenation
    int *ptr = GET_ADDR(\u04, A0);  // Line 13 - VIOLATION
}

int main(void) {
    address_test();
    return 0;
}
