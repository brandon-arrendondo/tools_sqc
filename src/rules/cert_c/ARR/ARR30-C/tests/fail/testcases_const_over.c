/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Const array accessed beyond declared bounds (reading out of bounds)
 */

#include <stdio.h>

const int lookup_table[8] = {10, 20, 30, 40, 50, 60, 70, 80};

void use_lookup() {
    // Read beyond const array bounds
    printf("lookup_table[10] = %d\n", lookup_table[10]);  // Line 13 - VIOLATION
    int val = lookup_table[12];  // Line 14 - VIOLATION
    printf("Value: %d\n", val);
}

int main(void) {
    use_lookup();
    printf("lookup_table[15] = %d\n", lookup_table[15]);  // Line 21 - VIOLATION
    return 0;
}
