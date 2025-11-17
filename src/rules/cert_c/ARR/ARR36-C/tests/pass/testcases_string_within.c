/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: PASS
 * Reason: Pointer arithmetic within same string
 */

#include <stddef.h>
#include <stdio.h>

void string_arithmetic(void) {
    char message[] = "Hello, World!";
    char *start = message;
    char *comma = message + 5;
    char *end = message + 12;

    // All pointers refer to same string array - COMPLIANT
    ptrdiff_t len1 = comma - start;
    ptrdiff_t len2 = end - start;

    printf("Distance to comma: %td\n", len1);
    printf("Distance to end: %td\n", len2);

    if (start < end) {
        printf("Valid comparison\n");
    }
}

int main(void) {
    string_arithmetic();
    return 0;
}
