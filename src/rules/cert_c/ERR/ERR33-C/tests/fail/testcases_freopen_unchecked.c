/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: freopen() return value is not checked for NULL
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("test.txt", "w");
    if (file == NULL) {
        return 1;
    }

    fprintf(file, "Initial content\n");

    // VIOLATION: Return value not checked for NULL
    freopen("another.txt", "r", file);

    // Assuming freopen succeeded
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    printf("Read: %s", buffer);

    // Another unchecked freopen
    freopen("third.txt", "w", file);
    fprintf(file, "New content\n");

    fclose(file);
    return 0;
}