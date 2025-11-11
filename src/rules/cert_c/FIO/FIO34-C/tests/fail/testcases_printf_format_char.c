/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Format string processor with char type fails on extended chars
 */

#include <stdio.h>
#include <stdlib.h>

void process_format_string(FILE *file) {
    char c; // VIOLATION: char type cannot handle all format characters
    int in_format = 0;

    printf("Processing format string:\n");

    // Format string processing will fail on extended character sets
    while ((c = fgetc(file)) != EOF) {
        if (c == '%' && !in_format) {
            in_format = 1;
            printf("FORMAT START: %%");
        } else if (in_format) {
            printf("%c", c);
            if (c == 'd' || c == 's' || c == 'c' || c == 'f' || c == 'x') {
                printf(" FORMAT END\n");
                in_format = 0;
            }
        } else {
            printf("TEXT: %c\n", c);
        }
    }
}

int main() {
    FILE *file = fopen("format_strings.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open format file\n");
        return 1;
    }

    process_format_string(file);

    fclose(file);
    return 0;
}