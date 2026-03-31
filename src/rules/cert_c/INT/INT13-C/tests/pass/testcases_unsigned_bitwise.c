/*
 * Rule: INT13-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT13-C violation
 * Description: Bitwise operations on unsigned types
 */

void unsigned_bitwise_ops(void) {
    unsigned int mask = 0xFFu;
    unsigned int value = 0x1234u;

    unsigned int result1 = value & mask;
    unsigned int result2 = value | 0x8000u;
    unsigned int result3 = value ^ mask;
    unsigned int result4 = ~value;
    unsigned int result5 = value >> 8;
    unsigned int result6 = value << 4;
}
