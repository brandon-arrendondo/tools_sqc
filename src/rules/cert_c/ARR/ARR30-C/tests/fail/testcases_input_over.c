/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: User input used as array index without validation
 */

#include <stdio.h>

int main(void) {
    int data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int index;

    printf("Enter array index: ");
    scanf("%d", &index);

    // No bounds checking on user input
    printf("data[%d] = %d\n", index, data[index]);
    data[index] = 999;

    return 0;
}