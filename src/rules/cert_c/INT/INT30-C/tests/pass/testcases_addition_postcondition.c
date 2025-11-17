/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: Addition with postcondition check for wrap
 */

void func(unsigned int ui_a, unsigned int ui_b) {
    unsigned int usum = ui_a + ui_b;

    // Postcondition test - COMPLIANT
    if (usum < ui_a) {
        // Handle error - wrapped
        return;
    }

    // Use usum...
}

int main(void) {
    func(100, 200);
    return 0;
}
