/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR32-C violation
 */

#include <stdio.h>

#define MAX_VLA_SIZE 1000

void safe_vla_function(size_t size) {
    if (size == 0 || size > MAX_VLA_SIZE) {
        printf("Invalid VLA size: %zu\n", size);
        return;
    }
    
    int array[size];
    
    for (size_t i = 0; i < size; i++) {
        array[i] = i;
    }
    
    printf("Created safe VLA of size %zu\n", size);
}

int main() {
    safe_vla_function(100);
    return 0;
}
