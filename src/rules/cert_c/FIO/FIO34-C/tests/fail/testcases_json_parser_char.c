/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Simple JSON parser with char type fails on Unicode data
 */

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    JSON_STRING,
    JSON_NUMBER,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_BOOLEAN,
    JSON_NULL
} JsonType;

void skip_whitespace(FILE *file) {
    char c; // VIOLATION: char type cannot handle all whitespace characters

    while ((c = fgetc(file)) != EOF) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            ungetc(c, file);
            break;
        }
    }
}

void parse_string(FILE *file) {
    char c; // VIOLATION: char type fails on UTF-8 sequences
    int escape = 0;

    printf("String: \"");

    // String parsing will fail on multi-byte UTF-8 characters
    while ((c = fgetc(file)) != EOF) {
        if (escape) {
            printf("\\%c", c);
            escape = 0;
        } else if (c == '\\') {
            escape = 1;
        } else if (c == '"') {
            break;
        } else {
            printf("%c", c);
        }
    }

    printf("\"\n");
}

int main() {
    FILE *file = fopen("data.json", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open JSON file\n");
        return 1;
    }

    char c;

    // Simple JSON parsing - will fail on Unicode content
    while ((c = fgetc(file)) != EOF) {
        skip_whitespace(file);
        c = fgetc(file);

        if (c == '"') {
            parse_string(file);
        } else if (c != EOF) {
            printf("Other: %c\n", c);
        }
    }

    fclose(file);
    return 0;
}