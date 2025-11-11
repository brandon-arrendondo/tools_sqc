/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Macro-generated array access creates out-of-bounds access
 */

#include <stdio.h>

#define ARRAY_SIZE 8
#define ACCESS_ELEMENT(arr, idx) arr[idx]
#define UNSAFE_ACCESS(arr, idx) arr[idx + 5]

int main(void) {
    int data[ARRAY_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};

    // Macro creates out-of-bounds access
    printf("Element: %d\n", UNSAFE_ACCESS(data, 6));
    UNSAFE_ACCESS(data, 7) = 999;

    // Direct macro misuse
    printf("Element: %d\n", ACCESS_ELEMENT(data, 15));

    return 0;
}