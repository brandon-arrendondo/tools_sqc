/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_VLA_SIZE 1000
#define MIN_VLA_SIZE 1

void process_data_with_vla(size_t size) {
    if (size < MIN_VLA_SIZE || size > MAX_VLA_SIZE) {
        printf("Invalid size: %zu. Must be between %d and %d\n",
               size, MIN_VLA_SIZE, MAX_VLA_SIZE);
        return;
    }

    int data[size];

    for (size_t i = 0; i < size; i++) {
        data[i] = i * i;
    }

    printf("Processed VLA of size %zu successfully\n", size);
}

int main() {
    size_t user_size;

    printf("Enter array size: ");
    if (scanf("%zu", &user_size) == 1) {
        process_data_with_vla(user_size);
    } else {
        printf("Invalid input\n");
    }

    return 0;
}