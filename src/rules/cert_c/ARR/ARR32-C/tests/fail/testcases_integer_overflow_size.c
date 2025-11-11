/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stdio.h>
#include <limits.h>

void create_overflow_array(void) {
    size_t base = SIZE_MAX / 2;
    size_t multiplier = 3;

    size_t size = base * multiplier;  // Overflow

    int array[size];

    printf("Created array with overflowed size\n");
}

int main() {
    create_overflow_array();
    return 0;
}