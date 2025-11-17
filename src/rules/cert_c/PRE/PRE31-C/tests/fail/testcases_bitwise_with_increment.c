/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Bitwise operation with increment in unsafe macro
 */

#define CHECK_BIT(x, bit) (((x) & (1 << (bit))) != 0)  /* UNSAFE */

void check_bits(int value) {
    int bit_pos = 3;

    // Increment in bitwise expression evaluated multiple times
    if (CHECK_BIT(value, bit_pos++)) {  // Line 13 - VIOLATION
        // bit_pos incremented multiple times
    }
}

int main(void) {
    check_bits(0x0F);
    return 0;
}
