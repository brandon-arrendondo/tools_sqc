/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Function dereferences parameter without NULL check
 */

#include <stdio.h>

void unsafe_function(int *ptr) {
    // No NULL check before dereference
    *ptr = 100;
    printf("Value set to: %d\n", *ptr);
}

int main() {
    unsafe_function(NULL);
    return 0;
}