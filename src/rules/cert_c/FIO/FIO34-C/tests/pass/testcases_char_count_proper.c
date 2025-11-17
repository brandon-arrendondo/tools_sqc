/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: Character counting function with proper EOF handling
 */

#include <stdio.h>
#include <stdlib.h>

size_t count_characters(FILE *file) {
    int c; // Correct: int type for character reading
    size_t count = 0;

    while ((c = fgetc(file)) != EOF || (!feof(file) && !ferror(file))) {
        if (c != EOF) {
            count++;
        }
    }

    return count;
}

int main() {
    FILE *file = fopen("document.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open document.txt\n");
        return 1;
    }

    size_t char_count = count_characters(file);

    if (ferror(file)) {
        fprintf(stderr, "Error occurred while counting characters\n");
        fclose(file);
        return 1;
    }

    printf("Total characters in file: %zu\n", char_count);

    fclose(file);
    return 0;
}