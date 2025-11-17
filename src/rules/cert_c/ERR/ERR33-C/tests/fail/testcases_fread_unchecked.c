/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: fread() return value is not checked for read errors or EOF
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("data.bin", "rb");
    if (file == NULL) {
        return 1;
    }

    char buffer[1024];

    // VIOLATION: Return value not checked
    fread(buffer, sizeof(char), sizeof(buffer), file);

    // Assuming read succeeded and using potentially uninitialized data
    printf("First byte: %d\n", buffer[0]);

    // Another unchecked fread
    fread(buffer, 1, 512, file);
    printf("Read more data supposedly\n");

    fclose(file);
    return 0;
}