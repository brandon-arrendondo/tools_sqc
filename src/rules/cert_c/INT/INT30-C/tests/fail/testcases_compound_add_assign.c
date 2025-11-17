/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Compound addition assignment without wrap check
 */

void accumulate(unsigned int *total, unsigned int value) {
    // Compound assignment may wrap
    *total += value;  // Line 9 - VIOLATION
}

int main(void) {
    unsigned int sum = 4000000000U;
    accumulate(&sum, 1000000000U);  // Will wrap
    return 0;
}
