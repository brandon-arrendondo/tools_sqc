/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: File copy using char type - will fail on binary files with 0xFF bytes
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *src = fopen("source.bin", "rb");
    FILE *dst = fopen("dest.bin", "wb");

    if (src == NULL || dst == NULL) {
        fprintf(stderr, "Could not open files\n");
        return 1;
    }

    char c; // VIOLATION: char type will cause premature termination

    // This copy will stop at the first 0xFF byte in the source file
    while ((c = fgetc(src)) != EOF) {
        fputc(c, dst);
    }

    printf("File copy completed\n");

    fclose(src);
    fclose(dst);
    return 0;
}