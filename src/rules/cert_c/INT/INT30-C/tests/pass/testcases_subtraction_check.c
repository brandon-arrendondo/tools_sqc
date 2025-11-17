/*
 * Rule: INT30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: PASS
 * Reason: Subtraction with underflow check
 */

void func(unsigned int ui_a, unsigned int ui_b) {
    unsigned int udiff;

    // Check for underflow - COMPLIANT
    if (ui_a < ui_b) {
        // Handle error
        return;
    }

    udiff = ui_a - ui_b;
    // Use udiff...
}

int main(void) {
    func(200, 100);
    return 0;
}
