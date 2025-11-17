/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Left shift causing wrap used in security context
 */

void bit_shift_unsafe(unsigned int value, unsigned int shift) {
    // Left shift may wrap - used in security-critical context
    unsigned int mask = value << shift;  // Line 9 - VIOLATION

    // Use mask in access control decision...
}

int main(void) {
    bit_shift_unsafe(0xFFFFFFFFU, 4);  // Will wrap
    return 0;
}
