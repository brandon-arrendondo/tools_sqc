/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: String operations exceed buffer boundaries
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    char buffer[10];
    char source[] = "This string is definitely too long for the buffer";

    // strcpy without bounds checking causes overflow
    strcpy(buffer, source);

    printf("Buffer: %s\n", buffer);
    return 0;
}