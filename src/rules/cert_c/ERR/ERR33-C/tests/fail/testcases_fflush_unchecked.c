/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: fflush() return value is not checked for EOF (failure)
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("buffer_test.txt", "w");
    if (file == NULL) {
        return 1;
    }

    fprintf(file, "Buffered data\n");

    // VIOLATION: Return value not checked for EOF
    fflush(file);

    printf("Buffer supposedly flushed\n");

    fprintf(file, "More data\n");

    // Another unchecked fflush
    fflush(file);

    fclose(file);
    return 0;
}