/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: fseek() return value is not checked for seek errors
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        return 1;
    }

    // VIOLATION: Return value not checked
    fseek(file, 100, SEEK_SET);

    // Assuming seek succeeded
    char buffer[10];
    fgets(buffer, sizeof(buffer), file);
    printf("Read after seek: %s", buffer);

    // Another unchecked fseek
    fseek(file, -50, SEEK_CUR);

    fclose(file);
    return 0;
}