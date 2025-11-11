/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using fgets result without checking for NULL
 */

#include <stdio.h>

int main() {
    char buffer[100];
    char *result = fgets(buffer, sizeof(buffer), stdin);

    // fgets can return NULL on error or EOF
    printf("First char: %c\n", result[0]);

    return 0;
}