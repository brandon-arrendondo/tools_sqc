/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: CSV parser with char type fails on extended character sets
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_csv_line(FILE *file) {
    char c; // VIOLATION: char type cannot handle all character values
    char field[256];
    int field_pos = 0;
    int field_count = 0;
    int in_quotes = 0;

    // CSV parsing will fail if data contains high-bit characters
    while ((c = fgetc(file)) != EOF && c != '\n') {
        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) {
            field[field_pos] = '\0';
            printf("Field %d: %s\n", ++field_count, field);
            field_pos = 0;
        } else {
            if (field_pos < sizeof(field) - 1) {
                field[field_pos++] = c;
            }
        }
    }

    // Output last field
    if (field_pos > 0) {
        field[field_pos] = '\0';
        printf("Field %d: %s\n", ++field_count, field);
    }
}

int main() {
    FILE *file = fopen("data.csv", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open CSV file\n");
        return 1;
    }

    int line_num = 1;
    char c;

    // Parse each line - will miss lines with high-bit characters
    while ((c = fgetc(file)) != EOF) {
        if (c != '\n') {
            // Put character back and parse the line
            ungetc(c, file);
            printf("Line %d:\n", line_num++);
            parse_csv_line(file);
        }
    }

    fclose(file);
    return 0;
}