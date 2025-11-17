/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Simple compression using char type loses data
 */

#include <stdio.h>
#include <stdlib.h>

void simple_rle_compress(FILE *input, FILE *output) {
    char current, previous; // VIOLATION: char types cause data loss
    int count = 1;

    if ((previous = fgetc(input)) == EOF) {
        return;
    }

    // RLE compression will fail on data containing 0xFF bytes
    while ((current = fgetc(input)) != EOF) {
        if (current == previous && count < 255) {
            count++;
        } else {
            fputc(count, output);
            fputc(previous, output);
            previous = current;
            count = 1;
        }
    }

    // Write final run
    fputc(count, output);
    fputc(previous, output);
}

int main() {
    FILE *input = fopen("input.dat", "rb");
    FILE *output = fopen("compressed.rle", "wb");

    if (input == NULL || output == NULL) {
        fprintf(stderr, "Could not open files\n");
        return 1;
    }

    simple_rle_compress(input, output);

    printf("Compression completed\n");

    fclose(input);
    fclose(output);
    return 0;
}