/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: Line reading with proper EOF and error detection
 */

#include <stdio.h>
#include <stdlib.h>

int read_line(FILE *file, char *buffer, size_t size) {
    if (size == 0) return -1;

    int c; // Correct: int for character reading
    size_t i = 0;

    while (i < size - 1) {
        c = fgetc(file);

        if (c == EOF) {
            if (feof(file)) {
                break; // End of file reached
            }
            if (ferror(file)) {
                return -1; // Error occurred
            }
        }

        if (c == '\n') {
            break; // End of line
        }

        buffer[i++] = (char)c;
    }

    buffer[i] = '\0';
    return (int)i;
}

int main() {
    FILE *file = fopen("lines.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open lines.txt\n");
        return 1;
    }

    char line_buffer[256];
    int line_num = 1;
    int result;

    while ((result = read_line(file, line_buffer, sizeof(line_buffer))) >= 0) {
        printf("Line %d: %s\n", line_num++, line_buffer);

        if (feof(file)) {
            break;
        }
    }

    if (result < 0 && ferror(file)) {
        fprintf(stderr, "Error reading file\n");
    }

    fclose(file);
    return 0;
}