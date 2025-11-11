/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR00-C violation
 */

#include <stdio.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void safe_array_copy(int *dest, size_t dest_size, const int *src, size_t src_size) {
    size_t copy_size = MIN(dest_size, src_size);

    for (size_t i = 0; i < copy_size; i++) {
        dest[i] = src[i];
    }
}

int main() {
    int source[5] = {1, 2, 3, 4, 5};
    int destination[10] = {0};

    safe_array_copy(destination,
                   sizeof(destination) / sizeof(destination[0]),
                   source,
                   sizeof(source) / sizeof(source[0]));

    printf("Copied array: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", destination[i]);
    }
    printf("\n");

    return 0;
}