/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Array access in union member beyond its allocated bounds
 */

#include <stdio.h>

typedef union {
    char bytes[4];
    int value;
} Data;

int main(void) {
    Data data;
    data.value = 0x12345678;

    // Access beyond the 4-byte union bounds
    printf("bytes[5] = %02x\n", data.bytes[5]);
    data.bytes[6] = 0xFF;

    // Also accessing with larger index
    printf("bytes[10] = %02x\n", data.bytes[10]);

    return 0;
}