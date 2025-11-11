/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: ungetc() return value is not checked for EOF (failure)
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        return 1;
    }

    int ch = fgetc(file);
    if (ch != EOF) {
        // VIOLATION: Return value not checked for EOF
        ungetc(ch, file);

        // Assuming ungetc succeeded
        printf("Character supposedly pushed back\n");

        // Read the character again
        ch = fgetc(file);
        printf("Re-read character: %c\n", ch);
    }

    // Another unchecked ungetc
    ungetc('X', file);
    printf("X supposedly pushed back\n");

    fclose(file);
    return 0;
}