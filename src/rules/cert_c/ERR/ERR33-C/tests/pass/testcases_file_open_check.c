/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: PASS
 * Reason: File opening return value is properly checked and handled
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("test.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file\n");
        return 1;
    }

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("Read: %s", buffer);
    } else {
        fprintf(stderr, "Failed to read from file\n");
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Failed to close file\n");
        return 1;
    }

    return 0;
}