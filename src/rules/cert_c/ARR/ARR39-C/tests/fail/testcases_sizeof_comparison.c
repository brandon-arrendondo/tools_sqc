/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof in pointer comparison causing incorrect bounds
 */

void sizeof_compare(void) {
    short array[100];
    short *ptr = array;

    // sizeof(array) in comparison - double-scaling
    while (ptr < array + sizeof(array)) {  // Line 12 - VIOLATION
        *ptr++ = 0;
    }
}

int main(void) {
    sizeof_compare();
    return 0;
}
