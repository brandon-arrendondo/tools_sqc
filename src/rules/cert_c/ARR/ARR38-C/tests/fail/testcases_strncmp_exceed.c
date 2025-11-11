/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: strncmp with count exceeding string buffers
 */

#include <string.h>

void strncmp_overrun(void) {
    char str1[8] = "Hello";
    char str2[8] = "Help";

    // Compares 50 bytes from 8-byte buffers
    int result = strncmp(str1, str2, 50);  // Line 13 - VIOLATION
}

int main(void) {
    strncmp_overrun();
    return 0;
}
