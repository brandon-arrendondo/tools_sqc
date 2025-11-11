/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

/*
 * ARR00-C FAIL Case: Basic buffer overflow
 *
 * This test case demonstrates a classic buffer overflow vulnerability
 * caused by incorrect loop bounds. The loop condition "i <= 10"
 * allows access to buffer[10], which is out of bounds for a 10-element array.
 *
 * Vulnerability details:
 * - Array has valid indices 0-9, but loop accesses index 10
 * - Writes beyond allocated buffer boundaries
 * - Can corrupt adjacent memory locations
 * - Classic example of off-by-one error
 *
 * Security impact:
 * - Memory corruption
 * - Potential arbitrary code execution
 * - Stack corruption (if buffer is on stack)
 * - Unpredictable program behavior
 *
 * Attack vectors:
 * - Stack smashing attacks
 * - Return address overwriting
 * - Adjacent variable corruption
 */

#include <stdio.h>

int main() {
    int buffer[10];  // Array with valid indices 0-9

    // VULNERABILITY: Loop condition allows access to buffer[10]
    // Should be "i < 10" instead of "i <= 10"
    for (int i = 0; i <= 10; i++) {
        buffer[i] = i;  // Out-of-bounds write when i == 10
    }

    return 0;
}