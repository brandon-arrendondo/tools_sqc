/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: strdup() return value is not checked for NULL before use
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char *original = "Hello, World!";

    // VIOLATION: Return value not checked for NULL
    char *copy = strdup(original);

    // Direct use without NULL check
    printf("Copy: %s\n", copy); // Potential NULL pointer dereference
    printf("Length: %zu\n", strlen(copy)); // Another potential crash

    // Another unchecked strdup
    char *another_copy = strdup("Another string");
    printf("Another copy: %s\n", another_copy);

    free(copy);
    free(another_copy);
    return 0;
}