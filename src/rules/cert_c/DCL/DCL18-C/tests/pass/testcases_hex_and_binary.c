/*
 * Rule: DCL18-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL18-C violation
 * Description: Hex and plain zero constants are not octal
 */

void proper_constants(void) {
    int zero = 0;         /* Plain zero, not octal */
    int hex = 0xFF;       /* Hexadecimal prefix */
    int hex2 = 0X1A;      /* Uppercase hex prefix */
    int decimal = 42;     /* No leading zero */
    int negative = -1;    /* Negative literal */
    unsigned long ul = 0UL;  /* Zero with type suffix */
}
