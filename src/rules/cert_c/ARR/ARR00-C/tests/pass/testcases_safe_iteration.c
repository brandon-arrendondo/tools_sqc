/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR00-C violation
 */

#include <stdio.h>

#define ARRAY_SIZE 10

int main() {
    int numbers[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++) {
        numbers[i] = i * i;
    }

    printf("Forward iteration: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    printf("Reverse iteration: ");
    for (int i = ARRAY_SIZE - 1; i >= 0; i--) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    size_t i = 0;
    while (i < ARRAY_SIZE) {
        numbers[i] = numbers[i] + 1;
        i++;
    }

    return 0;
}