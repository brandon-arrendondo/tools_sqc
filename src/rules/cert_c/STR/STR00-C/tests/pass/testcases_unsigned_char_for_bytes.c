/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: unsigned_char_for_bytes.c
 *
 * This case demonstrates compliant code that uses unsigned char
 * for byte operations and binary data handling, ensuring predictable
 * behavior regardless of the platform's char signedness.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    printf("Proper unsigned char usage for byte operations:\n\n");

    /* COMPLIANT: Using unsigned char for byte values */
    unsigned char byte_data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x00};  /* "Hello" */

    printf("Byte array contents:\n");
    for (size_t i = 0; i < sizeof(byte_data); i++) {
        printf("byte_data[%zu] = 0x%02X (%u)\n", i, byte_data[i], byte_data[i]);
    }

    /* COMPLIANT: Binary data manipulation with unsigned char */
    unsigned char binary_buffer[256];

    /* Initialize with pattern */
    for (int i = 0; i < 256; i++) {
        binary_buffer[i] = (unsigned char)i;
    }

    printf("\nFirst 16 bytes of pattern:\n");
    for (int i = 0; i < 16; i++) {
        printf("0x%02X ", binary_buffer[i]);
    }
    printf("\n");

    /* COMPLIANT: XOR encryption with unsigned char */
    unsigned char key = 0x5A;
    unsigned char plaintext[] = "Secret message for encryption";
    size_t text_length = strlen((char*)plaintext);

    printf("\nXOR encryption:\n");
    printf("Original: %s\n", (char*)plaintext);

    /* Encrypt */
    for (size_t i = 0; i < text_length; i++) {
        plaintext[i] ^= key;
    }

    printf("Encrypted (hex): ");
    for (size_t i = 0; i < text_length; i++) {
        printf("%02X ", plaintext[i]);
    }
    printf("\n");

    /* Decrypt */
    for (size_t i = 0; i < text_length; i++) {
        plaintext[i] ^= key;
    }

    printf("Decrypted: %s\n", (char*)plaintext);

    /* COMPLIANT: Checksum calculation with unsigned char */
    unsigned char data_for_checksum[] = {
        0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0
    };

    unsigned char checksum = 0;
    for (size_t i = 0; i < sizeof(data_for_checksum); i++) {
        checksum += data_for_checksum[i];
    }

    printf("\nChecksum calculation:\n");
    printf("Data: ");
    for (size_t i = 0; i < sizeof(data_for_checksum); i++) {
        printf("0x%02X ", data_for_checksum[i]);
    }
    printf("\nChecksum: 0x%02X\n", checksum);

    /* COMPLIANT: Bit manipulation with unsigned char */
    printf("\nBit manipulation:\n");

    unsigned char flags = 0x00;
    printf("Initial flags: 0x%02X\n", flags);

    /* Set individual bits */
    flags |= 0x01;  /* Bit 0 */
    flags |= 0x04;  /* Bit 2 */
    flags |= 0x80;  /* Bit 7 */

    printf("After setting bits 0, 2, 7: 0x%02X\n", flags);

    /* Test individual bits */
    for (int bit = 0; bit < 8; bit++) {
        if (flags & (1 << bit)) {
            printf("Bit %d is set\n", bit);
        }
    }

    /* COMPLIANT: Array indexing with unsigned char */
    printf("\nArray indexing with unsigned char:\n");

    int lookup_table[256];
    for (int i = 0; i < 256; i++) {
        lookup_table[i] = i * 2;
    }

    /* Test with various byte values */
    unsigned char indices[] = {0, 50, 100, 150, 200, 255};
    for (size_t i = 0; i < sizeof(indices); i++) {
        unsigned char index = indices[i];
        printf("lookup_table[%u] = %d\n", index, lookup_table[index]);
    }

    /* COMPLIANT: File I/O with unsigned char for binary data */
    const char *filename = "binary_test.dat";
    FILE *file = fopen(filename, "wb");

    if (file != NULL) {
        /* Write binary data */
        unsigned char binary_output[] = {
            0x00, 0x01, 0x02, 0x03, 0x80, 0x90, 0xA0, 0xFF
        };

        fwrite(binary_output, sizeof(unsigned char), sizeof(binary_output), file);
        fclose(file);

        printf("\nBinary file operations:\n");
        printf("Written %zu bytes to file\n", sizeof(binary_output));

        /* Read binary data back */
        file = fopen(filename, "rb");
        if (file != NULL) {
            unsigned char binary_input[10];
            size_t bytes_read = fread(binary_input, sizeof(unsigned char),
                                    sizeof(binary_input), file);

            printf("Read %zu bytes from file:\n", bytes_read);
            for (size_t i = 0; i < bytes_read; i++) {
                printf("0x%02X ", binary_input[i]);
            }
            printf("\n");

            fclose(file);
        }

        /* Clean up */
        remove(filename);
    }

    /* COMPLIANT: Memory operations with unsigned char */
    printf("\nMemory operations:\n");

    unsigned char source_mem[] = "Source data with high bytes \x80\x90\xA0";
    unsigned char dest_mem[50];

    /* Copy memory */
    memcpy(dest_mem, source_mem, sizeof(source_mem));

    printf("Memory copy successful\n");
    printf("Source: ");
    for (size_t i = 0; i < sizeof(source_mem); i++) {
        if (source_mem[i] >= 32 && source_mem[i] <= 126) {
            printf("%c", source_mem[i]);
        } else {
            printf("\\x%02X", source_mem[i]);
        }
    }
    printf("\n");

    /* COMPLIANT: Character frequency analysis */
    printf("\nCharacter frequency analysis:\n");

    const char *text = "Sample text for frequency analysis with various characters!";
    unsigned char frequency[256] = {0};

    /* Count character frequencies */
    for (size_t i = 0; text[i] != '\0'; i++) {
        unsigned char c = (unsigned char)text[i];  /* Explicit cast */
        frequency[c]++;
    }

    /* Display frequencies for printable characters */
    printf("Character frequencies (printable only):\n");
    for (int i = 32; i <= 126; i++) {
        if (frequency[i] > 0) {
            printf("'%c': %u times\n", i, frequency[i]);
        }
    }

    /* COMPLIANT: Hash calculation with unsigned char */
    printf("\nHash calculation:\n");

    const char *hash_input = "Data for hashing";
    unsigned char hash = 0;

    for (size_t i = 0; hash_input[i] != '\0'; i++) {
        unsigned char c = (unsigned char)hash_input[i];
        hash = hash * 31 + c;  /* Simple hash algorithm */
    }

    printf("Input: %s\n", hash_input);
    printf("Hash: 0x%02X\n", hash);

    return 0;
}