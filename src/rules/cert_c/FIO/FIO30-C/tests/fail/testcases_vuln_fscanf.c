/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User input used as format string in fscanf
 */

#include <stdio.h>

int main() {
    FILE *input_file = fopen("data.txt", "r");
    char format_string[50];
    char buffer[100];

    printf("Enter fscanf format: ");
    fgets(format_string, sizeof(format_string), stdin);

    if (input_file) {
        // VULNERABLE: user input as fscanf format
        fscanf(input_file, format_string, buffer);
        printf("Read: %s\n", buffer);
        fclose(input_file);
    }

    return 0;
}