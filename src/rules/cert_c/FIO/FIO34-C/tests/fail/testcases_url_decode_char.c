/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: URL decoder with char type fails on encoded bytes
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int hex_to_int(char hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
    return -1;
}

void url_decode(FILE *input, FILE *output) {
    char c; // VIOLATION: char type cannot handle all decoded values

    // URL decoding will fail when decoded bytes equal 0xFF
    while ((c = fgetc(input)) != EOF) {
        if (c == '%') {
            char hex1 = fgetc(input);
            char hex2 = fgetc(input);

            if (hex1 != EOF && hex2 != EOF) {
                int val1 = hex_to_int(hex1);
                int val2 = hex_to_int(hex2);

                if (val1 >= 0 && val2 >= 0) {
                    unsigned char decoded = (val1 << 4) | val2;
                    fputc(decoded, output);
                    continue;
                }
            }

            // Invalid encoding, output as-is
            fputc('%', output);
            if (hex1 != EOF) fputc(hex1, output);
            if (hex2 != EOF) fputc(hex2, output);
        } else if (c == '+') {
            fputc(' ', output);
        } else {
            fputc(c, output);
        }
    }
}

int main() {
    FILE *input = fopen("encoded_url.txt", "r");
    FILE *output = fopen("decoded_url.txt", "w");

    if (input == NULL || output == NULL) {
        fprintf(stderr, "Could not open files\n");
        return 1;
    }

    url_decode(input, output);

    printf("URL decoding completed\n");

    fclose(input);
    fclose(output);
    return 0;
}