/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Base64 decoder with char type fails on certain inputs
 */

#include <stdio.h>
#include <stdlib.h>

int base64_char_value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

void base64_decode(FILE *input, FILE *output) {
    char c; // VIOLATION: char type cannot handle all input values
    char quartet[4];
    int pos = 0;

    // Decoder will fail if input contains bytes that sign-extend to EOF
    while ((c = fgetc(input)) != EOF) {
        if (c == '=') break; // Padding
        if (base64_char_value(c) >= 0) {
            quartet[pos++] = c;
            if (pos == 4) {
                // Decode quartet
                int val = (base64_char_value(quartet[0]) << 18) |
                         (base64_char_value(quartet[1]) << 12) |
                         (base64_char_value(quartet[2]) << 6) |
                         base64_char_value(quartet[3]);

                fputc((val >> 16) & 0xFF, output);
                fputc((val >> 8) & 0xFF, output);
                fputc(val & 0xFF, output);
                pos = 0;
            }
        }
    }
}

int main() {
    FILE *input = fopen("encoded.b64", "r");
    FILE *output = fopen("decoded.bin", "wb");

    if (input == NULL || output == NULL) {
        fprintf(stderr, "Could not open files\n");
        return 1;
    }

    base64_decode(input, output);

    printf("Base64 decoding completed\n");

    fclose(input);
    fclose(output);
    return 0;
}