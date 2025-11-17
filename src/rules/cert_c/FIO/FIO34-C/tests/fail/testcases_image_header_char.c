/*
 * Rule: FIO34-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO34-C violation
 */

/*
 * Rule: FIO34-C - Distinguish between characters read from a file and EOF or WEOF
 * Status: FAIL
 * Reason: Image header parser with char type fails on binary headers
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char signature[4];
    unsigned int width;
    unsigned int height;
    unsigned char bits_per_pixel;
} ImageHeader;

int parse_image_header(FILE *file, ImageHeader *header) {
    char c; // VIOLATION: char type cannot handle all header bytes

    // Parse signature - will fail if signature contains 0xFF
    for (int i = 0; i < 4; i++) {
        if ((c = fgetc(file)) == EOF) {
            return -1;
        }
        header->signature[i] = c;
    }

    // Parse width (little-endian) - will fail on certain width values
    header->width = 0;
    for (int i = 0; i < 4; i++) {
        if ((c = fgetc(file)) == EOF) {
            return -1;
        }
        header->width |= ((unsigned char)c) << (i * 8);
    }

    // Parse height (little-endian)
    header->height = 0;
    for (int i = 0; i < 4; i++) {
        if ((c = fgetc(file)) == EOF) {
            return -1;
        }
        header->height |= ((unsigned char)c) << (i * 8);
    }

    // Parse bits per pixel
    if ((c = fgetc(file)) == EOF) {
        return -1;
    }
    header->bits_per_pixel = (unsigned char)c;

    return 0;
}

int main() {
    FILE *file = fopen("image.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open image file\n");
        return 1;
    }

    ImageHeader header;
    if (parse_image_header(file, &header) == 0) {
        printf("Image Header:\n");
        printf("Signature: %.4s\n", header.signature);
        printf("Width: %u\n", header.width);
        printf("Height: %u\n", header.height);
        printf("Bits per pixel: %u\n", header.bits_per_pixel);
    } else {
        printf("Failed to parse image header\n");
    }

    fclose(file);
    return 0;
}