/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Using char for fgetc return value - 0xFF bytes mistaken for EOF
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("binary_data.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file\n");
        return 1;
    }

    char c; // VIOLATION: char type cannot distinguish 0xFF from EOF

    printf("Reading binary file:\n");
    while ((c = fgetc(file)) != EOF) {
        printf("Byte: 0x%02X\n", (unsigned char)c);

        // If the file contains a byte with value 0xFF, this loop will
        // terminate early because 0xFF sign-extends to -1 (EOF)
    }

    printf("Finished reading file\n");
    fclose(file);
    return 0;
}