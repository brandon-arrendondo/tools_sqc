/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Comparing pointers within the same array using relational operators
 */

#include <stdio.h>

void compare_in_array(void) {
    int data[30] = {0};
    int *ptr1 = &data[10];
    int *ptr2 = &data[20];

    // Compare pointers within same array - COMPLIANT
    if (ptr1 < ptr2) {
        printf("ptr1 is before ptr2 in the array\n");
    }

    if (ptr2 >= ptr1) {
        printf("ptr2 is at or after ptr1\n");
    }
}

int main(void) {
    compare_in_array();
    return 0;
}
