/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: Proper EOF detection using int and feof/ferror validation
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("input.dat", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open binary file\n");
        return 1;
    }

    int c; // Correct: int can hold both character values and EOF
    size_t byte_count = 0;

    printf("Reading binary file:\n");
    while ((c = getc(file)) != EOF || (!feof(file) && !ferror(file))) {
        if (c != EOF) {
            printf("Byte %zu: 0x%02X (%d)\n", byte_count++, c, c);

            // Demonstrate that even 0xFF (255) is handled correctly
            if (c == 0xFF) {
                printf("  Note: This byte has same bit pattern as EOF on some systems\n");
            }
        }
    }

    printf("Total bytes read: %zu\n", byte_count);

    if (feof(file)) {
        printf("End of file reached normally\n");
    }
    if (ferror(file)) {
        fprintf(stderr, "Error occurred during reading\n");
    }

    fclose(file);
    return 0;
}