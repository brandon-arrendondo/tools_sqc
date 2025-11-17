/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Compound left shift in security-critical code without check
 */

void shift_mask(unsigned int *flags, unsigned int shift_amount) {
    // Compound left shift may wrap - security context
    *flags <<= shift_amount;  // Line 9 - VIOLATION

    // Use flags for permission checks...
}

int main(void) {
    unsigned int permissions = 0xFFFFU;
    shift_mask(&permissions, 20);  // Will wrap
    return 0;
}
