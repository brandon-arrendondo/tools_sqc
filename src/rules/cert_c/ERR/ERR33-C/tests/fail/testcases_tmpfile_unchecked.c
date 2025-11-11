/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: tmpfile() return value is not checked for NULL before use
 */

#include <stdio.h>

int main() {
    FILE *temp_file = tmpfile(); // VIOLATION: No NULL check

    // Direct use without checking if creation succeeded
    fprintf(temp_file, "Temporary data\n"); // Potential NULL pointer dereference

    rewind(temp_file); // Another potential NULL pointer dereference

    char buffer[256];
    fgets(buffer, sizeof(buffer), temp_file);
    printf("Read from temp file: %s", buffer);

    fclose(temp_file);
    return 0;
}