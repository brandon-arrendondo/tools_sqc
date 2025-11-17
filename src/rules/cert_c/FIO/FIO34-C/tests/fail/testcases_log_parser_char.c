/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Log file parser with char type misses entries with high-bit chars
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_log_entry(FILE *file) {
    char c; // VIOLATION: char type cannot handle all log characters
    char timestamp[32];
    char level[16];
    char message[512];
    int pos = 0;

    // Parse timestamp - will fail on logs with extended characters
    while ((c = fgetc(file)) != EOF && c != ' ' && pos < sizeof(timestamp) - 1) {
        timestamp[pos++] = c;
    }
    timestamp[pos] = '\0';

    // Skip space
    if (c == ' ') {
        pos = 0;
        // Parse log level
        while ((c = fgetc(file)) != EOF && c != ' ' && pos < sizeof(level) - 1) {
            level[pos++] = c;
        }
        level[pos] = '\0';

        // Skip space and parse message
        if (c == ' ') {
            pos = 0;
            while ((c = fgetc(file)) != EOF && c != '\n' && pos < sizeof(message) - 1) {
                message[pos++] = c;
            }
            message[pos] = '\0';

            printf("Log: [%s] %s - %s\n", timestamp, level, message);
        }
    }
}

int main() {
    FILE *file = fopen("application.log", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open log file\n");
        return 1;
    }

    char c;
    int entry_count = 0;

    // Parse log entries - will miss entries with extended characters
    while ((c = fgetc(file)) != EOF) {
        ungetc(c, file);
        parse_log_entry(file);
        entry_count++;
    }

    printf("Parsed %d log entries\n", entry_count);

    fclose(file);
    return 0;
}