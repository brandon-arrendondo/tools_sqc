/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Nested function calls can exceed buffer without proper size tracking
 */

#include <stdio.h>
#include <string.h>

void append_suffix(char *str) {
    strcat(str, "_processed");  // Adds 10 characters
}

void append_prefix(char *str, const char *prefix) {
    char temp[30];
    strcpy(temp, prefix);
    strcat(temp, str);
    strcpy(str, temp);  // Might overflow if str buffer is small
}

int main() {
    char data[20] = "data";

    append_prefix(data, "system_");  // Now "system_data" (11 chars)
    append_suffix(data);             // Tries to make "system_data_processed" (21 chars)
    printf("Result: %s\n", data);

    return 0;
}