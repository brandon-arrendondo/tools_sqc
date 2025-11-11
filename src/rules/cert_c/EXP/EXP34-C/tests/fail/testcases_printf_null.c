/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Passing NULL pointer to printf %s format specifier
 */

#include <stdio.h>

int main() {
    char *str = NULL;

    // Passing NULL to %s format specifier
    printf("String: %s\n", str);

    return 0;
}