/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Using array member in struct with proper pointer arithmetic
 */

#include <stddef.h>
#include <stdio.h>

struct numbers {
    short a[3];
};

int sum_numbers(const short *numb, size_t dim) {
    int total = 0;

    // Pointer arithmetic on actual array - COMPLIANT
    for (size_t i = 0; i < dim; ++i) {
        total += *(numb + i);
    }
    return total;
}

int main(void) {
    struct numbers my_numbers = { .a[0] = 1, .a[1] = 2, .a[2] = 3 };
    int sum = sum_numbers(my_numbers.a,
                          sizeof(my_numbers.a) / sizeof(my_numbers.a[0]));
    printf("Sum: %d\n", sum);
    return 0;
}
