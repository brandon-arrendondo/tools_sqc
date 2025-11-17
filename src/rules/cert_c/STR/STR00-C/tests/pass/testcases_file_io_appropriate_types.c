/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: file_io_appropriate_types.c
 *
 * This case demonstrates compliant code that uses appropriate character
 * types for file I/O operations, properly handling both text and binary
 * data with correct type usage.
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    printf("Proper character types for file I/O:\n\n");

    /* COMPLIANT: Text file operations with char */
    const char *text_filename = "sample_text.txt";
    const char *text_content = "Hello, World!\nThis is a sample text file.\nWith multiple lines.\n";

    printf("Text file operations:\n");

    /* Write text file */
    FILE *text_file = fopen(text_filename, "w");
    if (text_file != NULL) {
        fprintf(text_file, "%s", text_content);
        fclose(text_file);
        printf("Text file written successfully\n");
    }

    /* Read text file using proper character types */
    text_file = fopen(text_filename, "r");
    if (text_file != NULL) {
        /* COMPLIANT: Using int for character input to handle EOF */
        int c;
        int line_count = 1;
        int char_count = 0;

        printf("Reading text file character by character:\n");

        while ((c = fgetc(text_file)) != EOF) {
            char_count++;

            if (c == '\n') {
                line_count++;
                printf("\\n");
            } else if (c >= 32 && c <= 126) {  /* Printable ASCII */
                printf("%c", c);
            } else {
                printf("\\x%02X", c);
            }
        }

        printf("\n\nFile statistics:\n");
        printf("Lines: %d\n", line_count);
        printf("Characters: %d\n", char_count);

        /* Check for EOF vs error */
        if (feof(text_file)) {
            printf("File read completed successfully\n");
        } else if (ferror(text_file)) {
            printf("File read error occurred\n");
        }

        fclose(text_file);
    }

    /* COMPLIANT: Line-based text reading */
    text_file = fopen(text_filename, "r");
    if (text_file != NULL) {
        char line_buffer[256];
        int line_number = 1;

        printf("\nReading text file line by line:\n");

        while (fgets(line_buffer, sizeof(line_buffer), text_file) != NULL) {
            printf("Line %d: %s", line_number, line_buffer);
            line_number++;
        }

        fclose(text_file);
    }

    /* COMPLIANT: Binary file operations with unsigned char */
    const char *binary_filename = "sample_binary.dat";

    printf("\nBinary file operations:\n");

    /* Write binary data */
    FILE *binary_file = fopen(binary_filename, "wb");
    if (binary_file != NULL) {
        /* COMPLIANT: Using unsigned char for binary data */
        unsigned char binary_data[] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
            0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0,
            0xFE, 0xFF, 0x7F, 0x40, 0x20, 0x10
        };

        size_t written = fwrite(binary_data, sizeof(unsigned char),
                              sizeof(binary_data), binary_file);

        printf("Written %zu bytes to binary file\n", written);
        fclose(binary_file);
    }

    /* Read binary data */
    binary_file = fopen(binary_filename, "rb");
    if (binary_file != NULL) {
        unsigned char read_buffer[50];
        size_t bytes_read = fread(read_buffer, sizeof(unsigned char),
                                sizeof(read_buffer), binary_file);

        printf("Read %zu bytes from binary file:\n", bytes_read);

        for (size_t i = 0; i < bytes_read; i++) {
            printf("0x%02X ", read_buffer[i]);
            if ((i + 1) % 8 == 0) printf("\n");
        }
        if (bytes_read % 8 != 0) printf("\n");

        fclose(binary_file);
    }

    /* COMPLIANT: Mixed text/binary file handling */
    const char *mixed_filename = "mixed_data.dat";

    printf("\nMixed text/binary file operations:\n");

    /* Write mixed data */
    FILE *mixed_file = fopen(mixed_filename, "wb");
    if (mixed_file != NULL) {
        /* Write text header */
        const char *header = "HEADER";
        fwrite(header, sizeof(char), strlen(header), mixed_file);

        /* Write binary separator */
        unsigned char separator[] = {0x00, 0xFF, 0x00, 0xFF};
        fwrite(separator, sizeof(unsigned char), sizeof(separator), mixed_file);

        /* Write more text */
        const char *footer = "FOOTER";
        fwrite(footer, sizeof(char), strlen(footer), mixed_file);

        fclose(mixed_file);
        printf("Mixed data file written\n");
    }

    /* Read mixed data */
    mixed_file = fopen(mixed_filename, "rb");
    if (mixed_file != NULL) {
        /* COMPLIANT: Reading byte by byte with unsigned char */
        unsigned char byte;
        size_t position = 0;

        printf("Mixed file contents (byte by byte):\n");

        while (fread(&byte, sizeof(unsigned char), 1, mixed_file) == 1) {
            printf("Pos %zu: 0x%02X", position, byte);

            if (byte >= 32 && byte <= 126) {
                printf(" ('%c')", byte);
            }
            printf("\n");

            position++;
        }

        fclose(mixed_file);
    }

    /* COMPLIANT: File copying with appropriate types */
    printf("\nFile copying operations:\n");

    /* Copy text file */
    FILE *source = fopen(text_filename, "r");
    FILE *dest = fopen("copied_text.txt", "w");

    if (source != NULL && dest != NULL) {
        /* COMPLIANT: Character-based copying for text files */
        int ch;
        while ((ch = fgetc(source)) != EOF) {
            fputc(ch, dest);
        }

        printf("Text file copied successfully\n");
    }

    if (source != NULL) fclose(source);
    if (dest != NULL) fclose(dest);

    /* Copy binary file */
    source = fopen(binary_filename, "rb");
    dest = fopen("copied_binary.dat", "wb");

    if (source != NULL && dest != NULL) {
        /* COMPLIANT: Buffer-based copying for binary files */
        unsigned char copy_buffer[1024];
        size_t bytes_read;

        while ((bytes_read = fread(copy_buffer, sizeof(unsigned char),
                                 sizeof(copy_buffer), source)) > 0) {
            fwrite(copy_buffer, sizeof(unsigned char), bytes_read, dest);
        }

        printf("Binary file copied successfully\n");
    }

    if (source != NULL) fclose(source);
    if (dest != NULL) fclose(dest);

    /* COMPLIANT: File content analysis */
    printf("\nFile content analysis:\n");

    FILE *analysis_file = fopen(text_filename, "r");
    if (analysis_file != NULL) {
        int character_counts[256] = {0};
        int ch;

        /* Count character frequencies */
        while ((ch = fgetc(analysis_file)) != EOF) {
            /* COMPLIANT: Using int from fgetc, safe to use as array index */
            if (ch >= 0 && ch < 256) {
                character_counts[ch]++;
            }
        }

        printf("Character frequency analysis:\n");
        for (int i = 32; i <= 126; i++) {  /* Printable ASCII range */
            if (character_counts[i] > 0) {
                printf("'%c': %d times\n", i, character_counts[i]);
            }
        }

        /* Special characters */
        if (character_counts['\n'] > 0) {
            printf("Newlines: %d\n", character_counts['\n']);
        }
        if (character_counts['\t'] > 0) {
            printf("Tabs: %d\n", character_counts['\t']);
        }

        fclose(analysis_file);
    }

    /* COMPLIANT: File position operations */
    printf("\nFile positioning operations:\n");

    FILE *pos_file = fopen(binary_filename, "rb");
    if (pos_file != NULL) {
        /* Get file size */
        fseek(pos_file, 0, SEEK_END);
        long file_size = ftell(pos_file);
        printf("File size: %ld bytes\n", file_size);

        /* Read from different positions */
        for (int pos = 0; pos < file_size && pos < 10; pos += 3) {
            fseek(pos_file, pos, SEEK_SET);

            unsigned char byte;
            if (fread(&byte, sizeof(unsigned char), 1, pos_file) == 1) {
                printf("Byte at position %d: 0x%02X\n", pos, byte);
            }
        }

        fclose(pos_file);
    }

    /* Cleanup temporary files */
    remove(text_filename);
    remove(binary_filename);
    remove(mixed_filename);
    remove("copied_text.txt");
    remove("copied_binary.dat");

    printf("\nTemporary files cleaned up\n");

    return 0;
}