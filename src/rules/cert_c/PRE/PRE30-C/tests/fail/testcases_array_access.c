/*
 * Rule: PRE30-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE30-C violation
 */

/*
 * Rule: PRE30-C - Do not create a universal character name through concatenation
 * Status: FAIL
 * Reason: Creating UCN for array access through concatenation
 */

#define ACCESS_ARRAY(arr, idx) arr##idx[0]  // Line 7 - VIOLATION

void array_test(void) {
    int \u0430[5] = {1, 2, 3, 4, 5};  // Cyrillic small letter a

    // Creates \u0430 through concatenation
    int val = ACCESS_ARRAY(\u04, 30);  // Line 13 - VIOLATION
}

int main(void) {
    array_test();
    return 0;
}
