/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: safe_unsigned_char_exception.c
 *
 * This case demonstrates the exception to EXP33-C where reading
 * uninitialized unsigned char that could not have been a register
 * variable is defined behavior.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* COMPLIANT: Exception case - unsigned char array copying */
void safe_unsigned_char_operations(void) {
    /* The exception applies to unsigned char that could not be register storage class */
    unsigned char buffer1[100];  /* Not initialized, but this is the exception case */
    unsigned char buffer2[100];

    /* According to the exception, reading uninitialized unsigned char
     * that could not have been a register variable has defined behavior */

    /* However, best practice is still to initialize */
    memset(buffer2, 0, sizeof(buffer2));

    /* Copy some data to buffer1 first to make it more meaningful */
    const char *message = "Hello, World!";
    size_t msg_len = strlen(message);
    if (msg_len < sizeof(buffer1)) {
        memcpy(buffer1, message, msg_len + 1);
    }

    printf("Buffer1 content: %s\n", (char*)buffer1);

    /* Safe byte-level operations on unsigned char */
    for (size_t i = 0; i < msg_len && i < sizeof(buffer1); i++) {
        buffer2[i] = buffer1[i];  /* Byte copying is safe with unsigned char */
    }

    printf("Buffer2 content: %s\n", (char*)buffer2);
}

/* COMPLIANT: Proper unsigned char initialization (recommended approach) */
void recommended_unsigned_char_usage(void) {
    unsigned char data[256];

    /* Even though the exception allows reading uninitialized unsigned char,
     * it's still best practice to initialize explicitly */
    memset(data, 0, sizeof(data));

    /* Fill with some pattern */
    for (int i = 0; i < 10; i++) {
        data[i] = (unsigned char)(i + 65);  /* ASCII A, B, C, etc. */
    }

    printf("Initialized unsigned char data: ");
    for (int i = 0; i < 10; i++) {
        printf("%c ", data[i]);
    }
    printf("\n");

    /* Safe to read any byte in the array */
    printf("Byte values: ");
    for (int i = 0; i < 15; i++) {  /* Reading beyond initialized area */
        printf("%d ", data[i]);
    }
    printf("\n");
}

/* COMPLIANT: Binary data processing with unsigned char */
void safe_binary_data_processing(void) {
    /* Allocate and initialize binary buffer */
    unsigned char *binary_data = malloc(64);
    if (binary_data == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    /* Initialize the buffer explicitly (recommended) */
    memset(binary_data, 0, 64);

    /* Fill with binary pattern */
    for (int i = 0; i < 32; i++) {
        binary_data[i] = (unsigned char)(i * 8);
    }

    /* Safe processing of binary data */
    printf("Binary data processing:\n");
    for (int i = 0; i < 64; i += 8) {
        printf("Offset %02d: ", i);
        for (int j = 0; j < 8 && (i + j) < 64; j++) {
            printf("%02X ", binary_data[i + j]);
        }
        printf("\n");
    }

    free(binary_data);
}

/* COMPLIANT: File I/O with unsigned char buffers */
void safe_file_io_operations(void) {
    const char *filename = "test_data.bin";
    unsigned char write_buffer[50];
    unsigned char read_buffer[50];

    /* Initialize write buffer */
    for (int i = 0; i < 50; i++) {
        write_buffer[i] = (unsigned char)(i % 256);
    }

    /* Write data to file */
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Could not create test file\n");
        return;
    }

    size_t written = fwrite(write_buffer, 1, sizeof(write_buffer), file);
    fclose(file);

    if (written != sizeof(write_buffer)) {
        printf("Write operation incomplete\n");
        unlink(filename);
        return;
    }

    /* Initialize read buffer before reading */
    memset(read_buffer, 0xFF, sizeof(read_buffer));  /* Initialize to non-zero pattern */

    /* Read data from file */
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Could not open test file for reading\n");
        unlink(filename);
        return;
    }

    size_t read_count = fread(read_buffer, 1, sizeof(read_buffer), file);
    fclose(file);

    /* Verify data integrity */
    printf("File I/O verification:\n");
    printf("Bytes read: %zu\n", read_count);

    int matches = 0;
    for (size_t i = 0; i < read_count; i++) {
        if (read_buffer[i] == write_buffer[i]) {
            matches++;
        }
    }

    printf("Data integrity: %d/%zu bytes match\n", matches, read_count);

    /* Display some data */
    printf("First 16 bytes: ");
    for (int i = 0; i < 16 && i < (int)read_count; i++) {
        printf("%02X ", read_buffer[i]);
    }
    printf("\n");

    /* Cleanup */
    unlink(filename);
}

/* COMPLIANT: Memory comparison with unsigned char */
void safe_memory_comparison(void) {
    unsigned char array1[20];
    unsigned char array2[20];

    /* Initialize both arrays */
    memset(array1, 0xAA, sizeof(array1));
    memset(array2, 0xAA, sizeof(array2));

    /* Modify a few bytes in array2 */
    array2[5] = 0xBB;
    array2[10] = 0xCC;
    array2[15] = 0xDD;

    /* Safe byte-by-byte comparison */
    printf("Memory comparison results:\n");
    for (int i = 0; i < 20; i++) {
        if (array1[i] != array2[i]) {
            printf("Difference at byte %d: 0x%02X vs 0x%02X\n",
                   i, array1[i], array2[i]);
        }
    }

    /* Use standard library function for comparison */
    int cmp_result = memcmp(array1, array2, sizeof(array1));
    printf("memcmp result: %d (arrays are %s)\n",
           cmp_result, (cmp_result == 0) ? "equal" : "different");
}

/* COMPLIANT: String processing with explicit unsigned char casting */
void safe_string_byte_processing(void) {
    const char *text = "Hello, 世界! 123";
    size_t len = strlen(text);

    printf("String byte analysis:\n");
    printf("String: %s\n", text);
    printf("Length: %zu bytes\n", len);

    /* Process each byte as unsigned char */
    printf("Byte values: ");
    for (size_t i = 0; i < len; i++) {
        unsigned char byte = (unsigned char)text[i];
        printf("%02X ", byte);
    }
    printf("\n");

    /* Character classification on bytes */
    printf("Character analysis:\n");
    for (size_t i = 0; i < len; i++) {
        unsigned char byte = (unsigned char)text[i];
        printf("Byte %zu (0x%02X): ", i, byte);

        if (byte >= 32 && byte <= 126) {
            printf("printable ASCII '%c'\n", byte);
        } else if (byte < 32) {
            printf("control character\n");
        } else {
            printf("extended/non-ASCII\n");
        }
    }
}

int main(void) {
    printf("=== Safe Unsigned Char Exception Demo ===\n");

    printf("1. Unsigned char operations (exception case):\n");
    safe_unsigned_char_operations();

    printf("\n2. Recommended unsigned char usage:\n");
    recommended_unsigned_char_usage();

    printf("\n3. Binary data processing:\n");
    safe_binary_data_processing();

    printf("\n4. File I/O operations:\n");
    safe_file_io_operations();

    printf("\n5. Memory comparison:\n");
    safe_memory_comparison();

    printf("\n6. String byte processing:\n");
    safe_string_byte_processing();

    return 0;
}