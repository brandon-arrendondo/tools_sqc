/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Stack-allocated array accessed beyond bounds
 */

#include <stdio.h>

void local_overflow() {
    int local_array[6] = {100, 200, 300, 400, 500, 600};
    int other_var = 42;

    // Access beyond local array bounds
    printf("local_array[8] = %d\n", local_array[8]);
    local_array[10] = 9999;

    printf("other_var = %d\n", other_var);
}

int main(void) {
    local_overflow();
    return 0;
}