/*
 * Rule: INT13-C
 * Source: testcases
 * Status: FAIL - Should trigger INT13-C violation
 * Description: Bitwise operations on signed integer types
 */

void signed_bitwise_ops(void) {
    int mask = 0xFF;
    int value = 0x1234;

    int result1 = value & mask;    /* Violation: signed operands */
    int result2 = value | 0x8000;  /* Violation: signed operand */
    int result3 = value ^ mask;    /* Violation: signed operands */
    int result4 = ~value;          /* Violation: signed operand */
}
