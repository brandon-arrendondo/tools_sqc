/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stdio.h>

void unsafe_vla_function(size_t size) {
    int array[size];  // No size validation
    
    printf("Created unsafe VLA of size %zu\n", size);
}

int main() {
    unsafe_vla_function(0);        // Zero size
    unsafe_vla_function(1000000);  // Huge size
    return 0;
}
