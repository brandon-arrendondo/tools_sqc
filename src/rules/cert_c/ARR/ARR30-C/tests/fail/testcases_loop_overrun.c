/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Loop condition allows access beyond array bounds
 */

#include <stdio.h>

int main(void) {
    int numbers[5] = {10, 20, 30, 40, 50};

    // Loop goes beyond array bounds (i <= 5 instead of i < 5)
    for (int i = 0; i <= 5; i++) {
        printf("numbers[%d] = %d\n", i, numbers[i]);
    }

    return 0;
}