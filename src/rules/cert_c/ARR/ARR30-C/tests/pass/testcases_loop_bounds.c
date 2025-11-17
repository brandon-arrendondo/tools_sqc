/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: Loop bounds are correctly set to array size
 */

#include <stdio.h>
#define ARRAY_SIZE 5

int main(void) {
    int numbers[ARRAY_SIZE] = {10, 20, 30, 40, 50};
    int sum = 0;

    // Proper loop bounds using array size
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum += numbers[i];
    }

    printf("Sum: %d\n", sum);
    return 0;
}