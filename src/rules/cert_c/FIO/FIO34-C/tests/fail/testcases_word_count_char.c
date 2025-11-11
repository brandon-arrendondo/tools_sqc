/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Word counting with char type gives incorrect results
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void count_words(FILE *file) {
    char c; // VIOLATION: char type causes undercounting
    int in_word = 0;
    int word_count = 0;
    int char_count = 0;
    int line_count = 1;

    // Word count will be wrong if file contains high-bit characters
    while ((c = fgetc(file)) != EOF) {
        char_count++;

        if (c == '\n') {
            line_count++;
            in_word = 0;
        } else if (isspace(c)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            word_count++;
        }
    }

    printf("Lines: %d\n", line_count);
    printf("Words: %d\n", word_count);
    printf("Characters: %d\n", char_count);
}

int main() {
    FILE *file = fopen("document.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    count_words(file);

    fclose(file);
    return 0;
}