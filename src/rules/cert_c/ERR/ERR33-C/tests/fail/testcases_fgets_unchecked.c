/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: fgets() return value is not checked for NULL/EOF conditions
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("test.txt", "r");
    if (file == NULL) {
        return 1;
    }

    char buffer[256];
    // VIOLATION: Return value not checked
    fgets(buffer, sizeof(buffer), file);

    // Direct use assuming read succeeded
    printf("Read: %s", buffer); // May contain garbage if read failed

    // Another unchecked fgets
    fgets(buffer, sizeof(buffer), file);
    printf("Second read: %s", buffer);

    fclose(file);
    return 0;
}