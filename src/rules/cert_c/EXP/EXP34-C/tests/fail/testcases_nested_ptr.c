/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Dereferencing nested NULL pointer
 */

#include <stdio.h>

typedef struct {
    int *value;
} Container;

int main() {
    Container c;
    c.value = NULL;

    // Dereferencing nested NULL pointer
    *(c.value) = 42;
    printf("Value: %d\n", *(c.value));

    return 0;
}