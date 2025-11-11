/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Parser using char type cannot handle all input bytes
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    PARSE_OK,
    PARSE_EOF,
    PARSE_ERROR
} ParseResult;

ParseResult parse_token(FILE *file, char *token, size_t max_len) {
    char c; // VIOLATION: char type causes parsing errors
    size_t len = 0;

    // Skip whitespace
    while ((c = fgetc(file)) != EOF && isspace(c)) {
        // Continue
    }

    if (c == EOF) {
        return PARSE_EOF;
    }

    // Read token - will fail if input contains 0xFF bytes
    while (c != EOF && !isspace(c) && len < max_len - 1) {
        token[len++] = c;
        c = fgetc(file);
    }

    token[len] = '\0';
    return PARSE_OK;
}

int main() {
    FILE *file = fopen("tokens.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    char token[256];
    ParseResult result;
    int token_count = 0;

    // Parser will miss tokens that contain high-bit characters
    while ((result = parse_token(file, token, sizeof(token))) == PARSE_OK) {
        printf("Token %d: %s\n", ++token_count, token);
    }

    printf("Total tokens parsed: %d\n", token_count);

    fclose(file);
    return 0;
}