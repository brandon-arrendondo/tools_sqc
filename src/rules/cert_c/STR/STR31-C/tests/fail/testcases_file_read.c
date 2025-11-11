/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Reading file content into fixed buffer without size checking
 */

#include <stdio.h>

int main() {
    FILE *file;
    char buffer[50];
    char line[200];  // Line might be longer than buffer

    file = fopen("data.txt", "r");
    if (file) {
        fgets(line, sizeof(line), file);
        strcpy(buffer, line);  // Line might exceed buffer capacity
        printf("Read: %s\n", buffer);
        fclose(file);
    }

    return 0;
}