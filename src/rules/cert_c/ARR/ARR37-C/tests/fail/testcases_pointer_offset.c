/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Using pointer offset on non-array variable
 */

#include <stdio.h>

void use_offset(void) {
    long value = 1234567L;
    long *ptr = &value;

    // Access with offset as if it were an array
    printf("%ld\n", ptr[1]);  // Line 14 - VIOLATION (equivalent to *(ptr + 1))
}

int main(void) {
    use_offset();
    return 0;
}
