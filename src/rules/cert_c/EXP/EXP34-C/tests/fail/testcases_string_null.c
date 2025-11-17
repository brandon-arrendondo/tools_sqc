/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using string functions on NULL pointer
 */

#include <stdio.h>
#include <string.h>

int main() {
    char *str = NULL;

    // Using strlen on NULL pointer
    size_t len = strlen(str);
    printf("Length: %zu\n", len);

    // Accessing character through NULL pointer
    printf("First char: %c\n", str[0]);

    return 0;
}