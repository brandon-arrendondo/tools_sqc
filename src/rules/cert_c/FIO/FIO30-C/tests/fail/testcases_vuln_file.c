/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Format string read from file and used directly
 */

#include <stdio.h>

int main() {
    FILE *config_file = fopen("format.conf", "r");
    char format_buffer[100];

    if (config_file) {
        if (fgets(format_buffer, sizeof(format_buffer), config_file)) {
            // VULNERABLE: file content as format string
            printf(format_buffer);
        }
        fclose(config_file);
    }

    return 0;
}