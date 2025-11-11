/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: Character filtering with proper EOF detection
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int filter_printable_chars(FILE *input, FILE *output) {
    int c; // Correct: int for character reading
    size_t filtered_count = 0;

    while ((c = fgetc(input)) != EOF || (!feof(input) && !ferror(input))) {
        if (c != EOF) {
            // Filter out non-printable characters except newlines and tabs
            if (isprint(c) || c == '\n' || c == '\t') {
                if (fputc(c, output) == EOF) {
                    return -1;
                }
            } else {
                filtered_count++;
            }
        }
    }

    if (ferror(input)) {
        return -1;
    }

    printf("Filtered out %zu non-printable characters\n", filtered_count);
    return 0;
}

int main() {
    FILE *input = fopen("input_with_binary.txt", "rb");
    if (input == NULL) {
        fprintf(stderr, "Could not open input file\n");
        return 1;
    }

    FILE *output = fopen("filtered_output.txt", "w");
    if (output == NULL) {
        fprintf(stderr, "Could not create output file\n");
        fclose(input);
        return 1;
    }

    if (filter_printable_chars(input, output) != 0) {
        fprintf(stderr, "Error during filtering\n");
        fclose(input);
        fclose(output);
        return 1;
    }

    fclose(input);
    fclose(output);

    printf("Character filtering completed successfully\n");
    return 0;
}