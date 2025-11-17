/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: byte_operations_char.c
 *
 * This case demonstrates a violation of STR00-C by using plain char
 * for byte operations where unsigned char would be more appropriate
 * to ensure predictable behavior across platforms.
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    /* VIOLATION: Using plain char for byte manipulation */
    char byte_buffer[256];
    char key = 0x5A;  /* Should use unsigned char for byte operations */

    /* Initialize buffer with byte values */
    for (int i = 0; i < 256; i++) {
        byte_buffer[i] = i;  /* Values > 127 have undefined signedness */
    }

    /* VIOLATION: XOR encryption with signed char */
    printf("Encrypting data with XOR...\n");
    for (int i = 0; i < 256; i++) {
        byte_buffer[i] ^= key;  /* Sign-dependent behavior */
    }

    /* VIOLATION: Byte value inspection with wrong type */
    printf("First 16 encrypted bytes:\n");
    for (int i = 0; i < 16; i++) {
        printf("0x%02X ", (unsigned char)byte_buffer[i]);  /* Cast needed */
    }
    printf("\n");

    /* VIOLATION: Checksum calculation with char */
    char checksum = 0;  /* Should be unsigned char */
    for (int i = 0; i < 256; i++) {
        checksum += byte_buffer[i];  /* Arithmetic with sign issues */
    }
    printf("Checksum: 0x%02X\n", (unsigned char)checksum);

    /* VIOLATION: Bit manipulation with plain char */
    char flags = 0;
    flags |= 0x80;    /* Setting high bit - sign dependent */
    flags |= 0x40;
    flags |= 0x20;

    printf("Flags value: 0x%02X\n", (unsigned char)flags);

    /* Check individual bits - problematic comparisons */
    if (flags & 0x80) {  /* May behave differently with signed char */
        printf("High bit is set\n");
    }

    /* VIOLATION: Array indexing with potentially negative char */
    char lookup_table[256];
    for (int i = 0; i < 256; i++) {
        lookup_table[i] = i / 2;
    }

    /* Using char as array index */
    for (int i = 200; i < 256; i++) {
        char index = i;  /* May be negative on signed char systems */
        /* This could cause undefined behavior */
        printf("lookup_table[%d] = %d\n", i, lookup_table[index]);
    }

    /* VIOLATION: Binary data processing */
    char binary_data[] = {0xFF, 0xFE, 0xFD, 0xFC, 0x00};

    printf("Binary data values:\n");
    for (size_t i = 0; i < sizeof(binary_data); i++) {
        printf("data[%zu] = %d (0x%02X)\n",
               i, binary_data[i], (unsigned char)binary_data[i]);
    }

    /* VIOLATION: Shift operations with char */
    char shift_value = 0x01;
    for (int i = 0; i < 8; i++) {
        printf("Shift %d: 0x%02X\n", i, (unsigned char)(shift_value << i));
    }

    return 0;
}