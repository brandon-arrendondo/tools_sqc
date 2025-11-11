/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: memcpy copies exact bytes without ensuring null termination
 */

#include <stdio.h>
#include <string.h>

int main() {
    char source[] = "Hello World";
    char dest[11];  // Exactly the length of source

    memcpy(dest, source, strlen(source));  // Doesn't copy null terminator
    // dest is not null-terminated
    printf("Copied: %s\n", dest);  // Undefined behavior

    return 0;
}