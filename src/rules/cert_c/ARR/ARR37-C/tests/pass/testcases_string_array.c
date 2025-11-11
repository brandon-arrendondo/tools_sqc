/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Pointer arithmetic on string (character array)
 */

#include <stdio.h>
#include <string.h>

void string_operations(void) {
    char message[] = "Hello, World!";
    char *ptr = message;

    // Pointer arithmetic on string array - COMPLIANT
    while (*ptr != '\0') {
        printf("%c", *ptr);
        ptr++;
    }
    printf("\n");

    // Reset and use offset - COMPLIANT
    ptr = message;
    printf("Character at index 7: %c\n", *(ptr + 7));
}

int main(void) {
    string_operations();
    return 0;
}
