/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: String pointer is checked before string operations
 */

#include <stdio.h>
#include <string.h>

size_t safe_strlen(const char *str) {
    if (str == NULL) {
        return 0;
    }
    return strlen(str);
}

int main() {
    const char *message = "Hello";
    const char *empty = NULL;

    printf("Length of message: %zu\n", safe_strlen(message));
    printf("Length of empty: %zu\n", safe_strlen(empty));

    if (message != NULL) {
        printf("First character: %c\n", message[0]);
    }

    return 0;
}