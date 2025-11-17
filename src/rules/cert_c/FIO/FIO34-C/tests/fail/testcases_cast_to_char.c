/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Casting return value to char loses EOF information
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    char c;

    printf("Reading file:\n");
    while ((c = (char)fgetc(file)) != EOF) { // VIOLATION: cast to char
        printf("Character: %c\n", c);
    }

    // The cast to char means that 0xFF bytes will be sign-extended
    // back to -1 when compared to EOF, causing premature termination

    fclose(file);
    return 0;
}