/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: fgets with size exceeding buffer
 */

#include <stdio.h>

void fgets_exceed(FILE *f) {
    char buffer[40];

    // Claims buffer can hold 100 bytes but it's only 40
    fgets(buffer, 100, f);  // Line 12 - VIOLATION
}

int main(void) {
    FILE *f = fopen("test.txt", "r");
    if (f) {
        fgets_exceed(f);
        fclose(f);
    }
    return 0;
}
