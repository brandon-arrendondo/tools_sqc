/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Byte comparison with char type gives wrong results
 */

#include <stdio.h>
#include <stdlib.h>

int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");

    if (f1 == NULL || f2 == NULL) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return -1;
    }

    char c1, c2; // VIOLATION: char types cannot handle all byte values
    size_t position = 0;

    // Comparison will be incorrect for files containing 0xFF bytes
    while ((c1 = fgetc(f1)) != EOF && (c2 = fgetc(f2)) != EOF) {
        if (c1 != c2) {
            printf("Files differ at position %zu\n", position);
            fclose(f1);
            fclose(f2);
            return 1;
        }
        position++;
    }

    // Check if both files ended at the same time
    char extra1 = fgetc(f1);
    char extra2 = fgetc(f2);

    fclose(f1);
    fclose(f2);

    if (extra1 != EOF || extra2 != EOF) {
        printf("Files have different lengths\n");
        return 1;
    }

    printf("Files are identical (%zu bytes)\n", position);
    return 0;
}

int main() {
    int result = compare_files("file1.bin", "file2.bin");

    if (result == 0) {
        printf("Files match\n");
    } else if (result == 1) {
        printf("Files differ\n");
    } else {
        printf("Error opening files\n");
    }

    return result;
}