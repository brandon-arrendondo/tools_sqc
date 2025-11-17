/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Compound subtraction assignment without underflow check
 */

void reduce_value(unsigned int *counter, unsigned int amount) {
    // Compound subtraction may wrap
    *counter -= amount;  // Line 9 - VIOLATION
}

int main(void) {
    unsigned int count = 100;
    reduce_value(&count, 200);  // Will underflow/wrap
    return 0;
}
