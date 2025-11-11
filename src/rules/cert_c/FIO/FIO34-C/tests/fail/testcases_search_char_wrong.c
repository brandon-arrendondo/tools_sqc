/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Character search function with char type fails on binary data
 */

#include <stdio.h>
#include <stdlib.h>

int find_character(FILE *file, char target) {
    char c; // VIOLATION: char type cannot handle all byte values
    size_t position = 0;

    // Will fail if searching through binary data containing 0xFF
    while ((c = fgetc(file)) != EOF) {
        if (c == target) {
            return position;
        }
        position++;
    }

    return -1; // Not found
}

int main() {
    FILE *file = fopen("data.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    char search_char = 'A';
    int pos = find_character(file, search_char);

    if (pos >= 0) {
        printf("Character '%c' found at position %d\n", search_char, pos);
    } else {
        printf("Character '%c' not found\n", search_char);
    }

    fclose(file);
    return 0;
}