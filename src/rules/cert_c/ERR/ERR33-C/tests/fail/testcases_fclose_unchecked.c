/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: fclose() return value is not checked for close errors
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("test.txt", "w");
    if (file == NULL) {
        return 1;
    }

    fprintf(file, "Some data\n");

    // VIOLATION: Return value not checked
    fclose(file);

    printf("File supposedly closed successfully\n");

    // Open another file
    file = fopen("another.txt", "w");
    if (file != NULL) {
        fprintf(file, "More data\n");
        // Another unchecked fclose
        fclose(file);
    }

    return 0;
}