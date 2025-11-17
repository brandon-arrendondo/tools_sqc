/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: ftell() return value is not checked for errors (-1)
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("position.txt", "r");
    if (file == NULL) {
        return 1;
    }

    // VIOLATION: Return value not checked for -1 error
    long position = ftell(file);

    // Using position assuming ftell succeeded
    printf("Current position: %ld\n", position); // May be -1 on error

    fseek(file, 50, SEEK_SET);

    // Another unchecked ftell
    position = ftell(file);
    printf("New position: %ld\n", position);

    fclose(file);
    return 0;
}