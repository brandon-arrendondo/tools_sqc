/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: PASS
 * Reason: Hex dump utility with proper EOF handling for binary data
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void print_hex_line(unsigned char *buffer, size_t length, size_t offset) {
    printf("%08zx  ", offset);

    // Print hex values
    for (size_t i = 0; i < 16; i++) {
        if (i < length) {
            printf("%02x ", buffer[i]);
        } else {
            printf("   ");
        }
        if (i == 7) printf(" ");
    }

    printf(" |");

    // Print ASCII representation
    for (size_t i = 0; i < length; i++) {
        char c = isprint(buffer[i]) ? buffer[i] : '.';
        printf("%c", c);
    }

    printf("|\n");
}

int hex_dump_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }

    int c; // Correct: int for character reading
    unsigned char buffer[16];
    size_t buffer_pos = 0;
    size_t total_offset = 0;

    while ((c = fgetc(file)) != EOF || (!feof(file) && !ferror(file))) {
        if (c != EOF) {
            buffer[buffer_pos++] = (unsigned char)c;

            if (buffer_pos == 16) {
                print_hex_line(buffer, buffer_pos, total_offset);
                total_offset += buffer_pos;
                buffer_pos = 0;
            }
        }
    }

    // Print remaining bytes if any
    if (buffer_pos > 0) {
        print_hex_line(buffer, buffer_pos, total_offset);
        total_offset += buffer_pos;
    }

    if (ferror(file)) {
        fclose(file);
        return -1;
    }

    printf("\nTotal bytes: %zu\n", total_offset);
    fclose(file);
    return 0;
}

int main() {
    const char *filename = "binary_data.bin";

    if (hex_dump_file(filename) != 0) {
        fprintf(stderr, "Error creating hex dump of %s\n", filename);
        return 1;
    }

    return 0;
}