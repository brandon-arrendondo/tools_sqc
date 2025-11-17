/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: Using ungetc properly with int character type and EOF checking
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int read_number(FILE *file, long *result) {
    int c; // Correct: int for character reading
    long value = 0;
    int sign = 1;
    int found_digits = 0;

    // Skip whitespace
    while ((c = fgetc(file)) != EOF && isspace(c)) {
        if (ferror(file)) return -1;
    }

    if (c == EOF) {
        if (feof(file)) return 0; // End of file
        if (ferror(file)) return -1; // Error
    }

    // Handle sign
    if (c == '-') {
        sign = -1;
        c = fgetc(file);
    } else if (c == '+') {
        c = fgetc(file);
    }

    // Read digits
    while (c != EOF && isdigit(c)) {
        value = value * 10 + (c - '0');
        found_digits = 1;
        c = fgetc(file);
        if (ferror(file)) return -1;
    }

    // Push back the non-digit character if we read one
    if (c != EOF) {
        if (ungetc(c, file) == EOF) {
            return -1; // ungetc failed
        }
    }

    if (!found_digits) {
        return 0; // No number found
    }

    *result = value * sign;
    return 1; // Success
}

int main() {
    FILE *file = fopen("numbers.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open numbers.txt\n");
        return 1;
    }

    long number;
    int result;
    int count = 0;

    printf("Reading numbers from file:\n");

    while ((result = read_number(file, &number)) > 0) {
        printf("Number %d: %ld\n", ++count, number);
    }

    if (result < 0) {
        fprintf(stderr, "Error reading numbers from file\n");
        fclose(file);
        return 1;
    }

    printf("Total numbers read: %d\n", count);
    fclose(file);
    return 0;
}