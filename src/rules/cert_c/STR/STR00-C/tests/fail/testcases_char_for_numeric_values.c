/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: char_for_numeric_values.c
 *
 * This case demonstrates a violation of STR00-C by using plain char
 * for numeric values where the signedness matters, leading to
 * unpredictable behavior across different platforms.
 */

#include <stdio.h>

int main(void) {
    /* VIOLATION: Using plain char for numeric values where signedness matters */
    char byte_values[10];

    /* Initialize with values that may be interpreted differently */
    for (int i = 0; i < 10; i++) {
        byte_values[i] = 128 + i;  /* Values > 127 - sign dependent */
    }

    printf("Byte values as signed integers:\n");
    for (int i = 0; i < 10; i++) {
        /* VIOLATION: Relying on sign-dependent behavior */
        printf("byte_values[%d] = %d\n", i, (int)byte_values[i]);
    }

    /* VIOLATION: Arithmetic operations on plain char with high values */
    char sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += byte_values[i];  /* Overflow and sign issues */
    }
    printf("Sum = %d\n", (int)sum);

    /* VIOLATION: Comparison operations that depend on signedness */
    for (int i = 0; i < 10; i++) {
        if (byte_values[i] > 0) {  /* Behavior depends on char signedness */
            printf("Positive value: %d\n", (int)byte_values[i]);
        } else {
            printf("Non-positive value: %d\n", (int)byte_values[i]);
        }
    }

    /* VIOLATION: Using char for bit manipulation */
    char flags = 0xFF;
    if (flags & 0x80) {  /* Sign-dependent behavior */
        printf("High bit set\n");
    }

    /* VIOLATION: Array indexing with potentially negative char */
    char lookup_table[256] = {0};
    char index = 200;  /* May be negative on signed char systems */
    lookup_table[index] = 42;  /* Undefined behavior if index is negative */

    return 0;
}