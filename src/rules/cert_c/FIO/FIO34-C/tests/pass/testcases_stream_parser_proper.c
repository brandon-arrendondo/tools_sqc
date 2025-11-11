/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: Stream parsing with proper EOF distinction and error handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
} Token;

int get_next_token(FILE *file, Token *token) {
    int c; // Correct: int for character reading
    size_t pos = 0;

    // Skip whitespace
    while ((c = fgetc(file)) != EOF && isspace(c)) {
        if (ferror(file)) {
            token->type = TOKEN_ERROR;
            return -1;
        }
    }

    if (c == EOF) {
        if (feof(file)) {
            token->type = TOKEN_EOF;
            return 0;
        }
        if (ferror(file)) {
            token->type = TOKEN_ERROR;
            return -1;
        }
    }

    // Parse number
    if (isdigit(c)) {
        token->type = TOKEN_NUMBER;
        do {
            if (pos < sizeof(token->value) - 1) {
                token->value[pos++] = c;
            }
            c = fgetc(file);
            if (ferror(file)) {
                token->type = TOKEN_ERROR;
                return -1;
            }
        } while (c != EOF && isdigit(c));

        // Push back non-digit
        if (c != EOF && ungetc(c, file) == EOF) {
            token->type = TOKEN_ERROR;
            return -1;
        }
    }
    // Parse identifier
    else if (isalpha(c) || c == '_') {
        token->type = TOKEN_IDENTIFIER;
        do {
            if (pos < sizeof(token->value) - 1) {
                token->value[pos++] = c;
            }
            c = fgetc(file);
            if (ferror(file)) {
                token->type = TOKEN_ERROR;
                return -1;
            }
        } while (c != EOF && (isalnum(c) || c == '_'));

        // Push back non-identifier character
        if (c != EOF && ungetc(c, file) == EOF) {
            token->type = TOKEN_ERROR;
            return -1;
        }
    }
    // Parse operator
    else {
        token->type = TOKEN_OPERATOR;
        if (pos < sizeof(token->value) - 1) {
            token->value[pos++] = c;
        }
    }

    token->value[pos] = '\0';
    return 1;
}

int main() {
    FILE *file = fopen("expression.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open expression.txt\n");
        return 1;
    }

    Token token;
    int result;
    int token_count = 0;

    printf("Parsing tokens from expression:\n");

    while ((result = get_next_token(file, &token)) > 0) {
        const char *type_name;
        switch (token.type) {
            case TOKEN_NUMBER: type_name = "NUMBER"; break;
            case TOKEN_IDENTIFIER: type_name = "IDENTIFIER"; break;
            case TOKEN_OPERATOR: type_name = "OPERATOR"; break;
            case TOKEN_EOF: type_name = "EOF"; break;
            default: type_name = "UNKNOWN"; break;
        }

        printf("Token %d: %s = '%s'\n", ++token_count, type_name, token.value);

        if (token.type == TOKEN_EOF) break;
    }

    if (result < 0) {
        fprintf(stderr, "Error parsing file\n");
        fclose(file);
        return 1;
    }

    printf("Total tokens parsed: %d\n", token_count);
    fclose(file);
    return 0;
}