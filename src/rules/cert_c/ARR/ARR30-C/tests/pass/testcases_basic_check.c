/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: PASS
 * Reason: Array access is properly bounds-checked before use
 */

#include <stdio.h>

int main(void) {
    int arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int index;

    printf("Enter an index (0-9): ");
    scanf("%d", &index);

    // Proper bounds checking
    if (index >= 0 && index < 10) {
        printf("arr[%d] = %d\n", index, arr[index]);
    } else {
        printf("Index out of bounds\n");
    }

    return 0;
}