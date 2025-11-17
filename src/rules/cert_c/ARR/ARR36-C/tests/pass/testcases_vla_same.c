/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Pointer operations within same VLA
 */

#include <stddef.h>
#include <stdio.h>

void vla_operations(int n) {
    int vla[n];
    int *start = vla;
    int *end = vla + n;

    // Pointers within same VLA - COMPLIANT
    ptrdiff_t size = end - start;
    printf("VLA size: %td\n", size);

    int *mid = vla + (n / 2);
    if (start <= mid && mid < end) {
        printf("Valid VLA pointer operations\n");
    }
}

int main(void) {
    vla_operations(25);
    return 0;
}
