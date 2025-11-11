/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Line reader using char type fails on binary data
 */

#include <stdio.h>
#include <stdlib.h>

int read_line(FILE *file, char *buffer, size_t size) {
    char c; // VIOLATION: char type cannot distinguish EOF properly
    size_t i = 0;

    while (i < size - 1 && (c = fgetc(file)) != EOF) {
        if (c == '\n') {
            break;
        }
        buffer[i++] = c;
    }

    buffer[i] = '\0';
    return i;
}

int main() {
    FILE *file = fopen("mixed_data.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    char line[256];
    int line_num = 1;

    // Will fail to read lines that contain 0xFF bytes
    while (read_line(file, line, sizeof(line)) > 0) {
        printf("Line %d: %s\n", line_num++, line);
    }

    fclose(file);
    return 0;
}