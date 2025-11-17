/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: File copying with proper EOF handling and error checking
 */

#include <stdio.h>
#include <stdlib.h>

int copy_file(const char *source, const char *dest) {
    FILE *src = fopen(source, "rb");
    if (src == NULL) {
        return -1;
    }

    FILE *dst = fopen(dest, "wb");
    if (dst == NULL) {
        fclose(src);
        return -1;
    }

    int c; // Correct: int for character reading
    size_t bytes_copied = 0;

    while ((c = fgetc(src)) != EOF || (!feof(src) && !ferror(src))) {
        if (c != EOF) {
            if (fputc(c, dst) == EOF) {
                fprintf(stderr, "Error writing to destination file\n");
                fclose(src);
                fclose(dst);
                return -1;
            }
            bytes_copied++;
        }
    }

    if (ferror(src)) {
        fprintf(stderr, "Error reading source file\n");
        fclose(src);
        fclose(dst);
        return -1;
    }

    fclose(src);
    fclose(dst);

    printf("Successfully copied %zu bytes\n", bytes_copied);
    return 0;
}

int main() {
    if (copy_file("source.dat", "destination.dat") != 0) {
        fprintf(stderr, "File copy failed\n");
        return 1;
    }

    printf("File copy completed successfully\n");
    return 0;
}