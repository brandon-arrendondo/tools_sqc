/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Escape sequence processor with char type fails on binary data
 */

#include <stdio.h>
#include <stdlib.h>

void process_escape_sequences(FILE *input, FILE *output) {
    char c; // VIOLATION: char type cannot handle all byte values
    int escape = 0;

    // Will fail to process escape sequences correctly if input contains 0xFF
    while ((c = fgetc(input)) != EOF) {
        if (escape) {
            switch (c) {
                case 'n': fputc('\n', output); break;
                case 't': fputc('\t', output); break;
                case 'r': fputc('\r', output); break;
                case '\\': fputc('\\', output); break;
                default:
                    fputc('\\', output);
                    fputc(c, output);
                    break;
            }
            escape = 0;
        } else if (c == '\\') {
            escape = 1;
        } else {
            fputc(c, output);
        }
    }
}

int main() {
    FILE *input = fopen("escaped_text.txt", "r");
    FILE *output = fopen("processed.txt", "w");

    if (input == NULL || output == NULL) {
        fprintf(stderr, "Could not open files\n");
        return 1;
    }

    process_escape_sequences(input, output);

    printf("Escape sequence processing completed\n");

    fclose(input);
    fclose(output);
    return 0;
}