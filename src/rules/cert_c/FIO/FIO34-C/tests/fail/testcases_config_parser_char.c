/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Configuration file parser with char type fails on Unicode values
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void parse_config_line(FILE *file) {
    char c; // VIOLATION: char type cannot handle all configuration values
    char key[128];
    char value[256];
    int pos = 0;
    int in_value = 0;

    // Skip leading whitespace and comments
    while ((c = fgetc(file)) != EOF && (isspace(c) || c == '#')) {
        if (c == '#') {
            // Skip comment line
            while ((c = fgetc(file)) != EOF && c != '\n') {
                // Continue
            }
            return;
        }
    }

    if (c == EOF) return;

    // Parse key - will fail on config files with extended characters
    ungetc(c, file);
    while ((c = fgetc(file)) != EOF && c != '=' && c != '\n' && pos < sizeof(key) - 1) {
        if (!isspace(c)) {
            key[pos++] = c;
        }
    }
    key[pos] = '\0';

    if (c == '=') {
        pos = 0;
        // Skip whitespace after =
        while ((c = fgetc(file)) != EOF && isspace(c) && c != '\n') {
            // Continue
        }

        // Parse value
        while (c != EOF && c != '\n' && pos < sizeof(value) - 1) {
            value[pos++] = c;
            c = fgetc(file);
        }
        value[pos] = '\0';

        // Trim trailing whitespace
        while (pos > 0 && isspace(value[pos - 1])) {
            value[--pos] = '\0';
        }

        printf("Config: %s = %s\n", key, value);
    }
}

int main() {
    FILE *file = fopen("config.ini", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open config file\n");
        return 1;
    }

    char c;

    // Parse configuration file - will miss Unicode configuration values
    while ((c = fgetc(file)) != EOF) {
        ungetc(c, file);
        parse_config_line(file);
    }

    fclose(file);
    return 0;
}