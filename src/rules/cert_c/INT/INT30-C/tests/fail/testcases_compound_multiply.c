/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Compound multiplication assignment without wrap check
 */

void scale_value(unsigned int *value, unsigned int multiplier) {
    // Compound multiply may wrap
    *value *= multiplier;  // Line 9 - VIOLATION
}

int main(void) {
    unsigned int val = 1000000U;
    scale_value(&val, 5000U);  // Will wrap
    return 0;
}
