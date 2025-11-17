/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Using equality operators on pointers to different arrays (allowed)
 */

#include <stdio.h>

void equality_test(void) {
    int array1[10] = {0};
    int array2[10] = {0};
    int *ptr1 = array1;
    int *ptr2 = array2;

    // Equality operators are allowed for unrelated pointers - COMPLIANT
    if (ptr1 == ptr2) {
        printf("Pointers are equal\n");
    } else {
        printf("Pointers are not equal\n");
    }

    if (ptr1 != ptr2) {
        printf("Pointers are different\n");
    }
}

int main(void) {
    equality_test();
    return 0;
}
