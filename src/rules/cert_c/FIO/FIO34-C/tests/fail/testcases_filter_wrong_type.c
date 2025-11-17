/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Character filtering with wrong type loses data
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("filtered.txt", "w");

    if (input == NULL || output == NULL) {
        fprintf(stderr, "Could not open files\n");
        return 1;
    }

    char c; // VIOLATION: char type causes data loss

    printf("Filtering characters:\n");

    // Will skip all 0xFF bytes in the input file
    while ((c = fgetc(input)) != EOF) {
        if (isprint(c) || c == '\n') {
            fputc(c, output);
        }
    }

    printf("Filtering completed\n");

    fclose(input);
    fclose(output);
    return 0;
}