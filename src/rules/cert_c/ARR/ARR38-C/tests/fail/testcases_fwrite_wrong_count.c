/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: fwrite with swapped size and count parameters
 */

#include <stdio.h>

void fwrite_swapped(FILE *f) {
    int data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Wrong: size=10, count=sizeof(int), should be reversed
    fwrite(data, 10, sizeof(int), f);  // Line 13 - VIOLATION
}

int main(void) {
    FILE *f = fopen("test.dat", "wb");
    if (f) {
        fwrite_swapped(f);
        fclose(f);
    }
    return 0;
}
