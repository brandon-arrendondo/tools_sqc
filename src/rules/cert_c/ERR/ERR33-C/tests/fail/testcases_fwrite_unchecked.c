/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: fwrite() return value is not checked for write errors
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        return 1;
    }

    char data[] = "Hello, World!";

    // VIOLATION: Return value not checked
    fwrite(data, sizeof(char), sizeof(data), file);

    // Assuming write succeeded without verification
    printf("Data supposedly written\n");

    // Another unchecked fwrite
    fwrite("More data", 1, 9, file);

    fclose(file);
    return 0;
}