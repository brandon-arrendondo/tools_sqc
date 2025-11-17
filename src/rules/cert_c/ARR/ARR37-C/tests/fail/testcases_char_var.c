/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single char variable
 */

#include <stdio.h>

void char_arithmetic(void) {
    char c = 'A';
    char *ptr = &c;

    // Increment pointer to single char
    ptr++;  // Line 14 - VIOLATION
    printf("%c\n", *ptr);  // Undefined behavior
}

int main(void) {
    char_arithmetic();
    return 0;
}
