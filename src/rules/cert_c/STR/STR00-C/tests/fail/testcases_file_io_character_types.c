/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: file_io_character_types.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types for file I/O operations, leading to data loss and
 * incorrect handling of character data.
 */

#include <stdio.h>

int main(void) {
    const char *filename = "test_char_io.txt";
    FILE *file;

    /* VIOLATION: Using char for file character operations */
    char file_char;  /* Should be int to handle EOF properly */

    /* Create test file */
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error creating file");
        return 1;
    }

    /* Write test data including high-bit characters */
    const char test_data[] = "Hello\x80\x90\xA0\xFF";
    fwrite(test_data, 1, sizeof(test_data) - 1, file);
    fclose(file);

    /* VIOLATION: Reading file with char instead of int */
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    printf("Reading file with char type:\n");
    while ((file_char = fgetc(file)) != EOF) {  /* Cannot detect EOF properly */
        /* VIOLATION: char cannot distinguish between 0xFF and EOF */
        printf("Read character: 0x%02X (%d)\n",
               (unsigned char)file_char, file_char);

        /* May terminate prematurely on 0xFF character */
        if (file_char == EOF) {  /* Comparison may fail */
            printf("EOF detected (possibly false)\n");
            break;
        }
    }
    fclose(file);

    /* VIOLATION: Binary file I/O with wrong character types */
    signed char signed_buffer[100];
    unsigned char unsigned_buffer[100];

    file = fopen(filename, "rb");
    if (file != NULL) {
        /* VIOLATION: Reading binary data into signed char */
        size_t signed_read = fread(signed_buffer, sizeof(signed char), 50, file);

        /* VIOLATION: Position-dependent behavior */
        fseek(file, 0, SEEK_SET);

        /* VIOLATION: Reading same data into unsigned char */
        size_t unsigned_read = fread(unsigned_buffer, sizeof(unsigned char), 50, file);

        fclose(file);

        printf("\nBinary file reading comparison:\n");
        printf("Signed read count: %zu\n", signed_read);
        printf("Unsigned read count: %zu\n", unsigned_read);

        /* Compare the data */
        for (size_t i = 0; i < signed_read && i < 10; i++) {
            printf("Position %zu: signed=%d, unsigned=%d\n",
                   i, signed_buffer[i], unsigned_buffer[i]);
        }
    }

    /* VIOLATION: Character output with wrong types */
    file = fopen("output_test.txt", "w");
    if (file != NULL) {
        signed char signed_output[] = "Signed output\x80\xFF";
        unsigned char unsigned_output[] = "Unsigned output\x80\xFF";

        /* VIOLATION: Writing signed char array to file */
        for (size_t i = 0; signed_output[i] != '\0'; i++) {
            fputc(signed_output[i], file);  /* Type warning */
        }

        fprintf(file, "\n");

        /* VIOLATION: Writing unsigned char array to file */
        for (size_t i = 0; unsigned_output[i] != '\0'; i++) {
            fputc(unsigned_output[i], file);  /* Type warning */
        }

        fclose(file);
    }

    /* VIOLATION: Text vs binary mode confusion */
    file = fopen("mixed_mode.txt", "w");
    if (file != NULL) {
        char mixed_data[] = {72, 101, 108, 108, 111, 13, 10, 0x80, 0};

        /* VIOLATION: Writing binary data in text mode */
        for (size_t i = 0; mixed_data[i] != '\0'; i++) {
            /* Character interpretation depends on text/binary mode */
            fputc(mixed_data[i], file);
        }

        fclose(file);
    }

    /* VIOLATION: Wide character file I/O with wrong types */
    file = fopen("wide_test.txt", "w");
    if (file != NULL) {
        wchar_t wide_chars[] = L"Wide: αβγ";

        /* VIOLATION: Writing wide characters as narrow characters */
        for (size_t i = 0; wide_chars[i] != L'\0'; i++) {
            fputc((char)wide_chars[i], file);  /* Data loss */
        }

        fclose(file);
    }

    /* VIOLATION: Character buffer operations */
    char input_buffer[256];

    /* Reading line with potential character type issues */
    file = fopen(filename, "r");
    if (file != NULL) {
        /* VIOLATION: Using char buffer for potentially binary data */
        if (fgets(input_buffer, sizeof(input_buffer), file) != NULL) {
            printf("\nLine read: ");
            for (size_t i = 0; input_buffer[i] != '\0' && input_buffer[i] != '\n'; i++) {
                printf("0x%02X ", (unsigned char)input_buffer[i]);
            }
            printf("\n");
        }
        fclose(file);
    }

    /* Cleanup */
    remove(filename);
    remove("output_test.txt");
    remove("mixed_mode.txt");
    remove("wide_test.txt");

    return 0;
}