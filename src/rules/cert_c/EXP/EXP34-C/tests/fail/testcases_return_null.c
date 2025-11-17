/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using function return value without checking for NULL
 */

#include <stdio.h>
#include <stdlib.h>

int* create_int() {
    return NULL;  // Function returns NULL
}

int main() {
    int *ptr = create_int();

    // Using return value without NULL check
    *ptr = 42;
    printf("Value: %d\n", *ptr);

    return 0;
}