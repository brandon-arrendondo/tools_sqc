/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Binary file search with char type fails on 0xFF patterns
 */

#include <stdio.h>
#include <stdlib.h>

long find_pattern(FILE *file, const char *pattern, size_t pattern_len) {
    char c; // VIOLATION: char type cannot handle all byte values
    size_t match_pos = 0;
    long file_pos = 0;

    // Pattern search will fail if pattern or file contains 0xFF bytes
    while ((c = fgetc(file)) != EOF) {
        if (c == pattern[match_pos]) {
            match_pos++;
            if (match_pos == pattern_len) {
                return file_pos - pattern_len + 1;
            }
        } else {
            match_pos = 0;
            if (c == pattern[0]) {
                match_pos = 1;
            }
        }
        file_pos++;
    }

    return -1; // Pattern not found
}

int main() {
    FILE *file = fopen("binary_data.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    const char pattern[] = {0x12, 0x34, 0xFF, 0xAB}; // Contains 0xFF
    long position = find_pattern(file, pattern, sizeof(pattern));

    if (position >= 0) {
        printf("Pattern found at position %ld\n", position);
    } else {
        printf("Pattern not found\n");
    }

    fclose(file);
    return 0;
}