/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Text processor using char type fails on extended ASCII
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void process_text(FILE *input) {
    char c; // VIOLATION: char type cannot handle extended ASCII
    int line_num = 1;
    int char_count = 0;

    printf("Processing text file:\n");

    // Will miss characters with high bit set (extended ASCII)
    while ((c = fgetc(input)) != EOF) {
        char_count++;

        if (c == '\n') {
            printf("Line %d: %d characters\n", line_num++, char_count - 1);
            char_count = 0;
        } else if (!isprint(c)) {
            printf("Non-printable character at line %d\n", line_num);
        }
    }

    if (char_count > 0) {
        printf("Line %d: %d characters\n", line_num, char_count);
    }
}

int main() {
    FILE *file = fopen("extended_text.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    process_text(file);

    fclose(file);
    return 0;
}