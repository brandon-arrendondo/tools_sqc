/*
 * Rule: INT00-C
 * Source: testcases
 * Status: FAIL - Unsafe cast + multiplication patterns
 */

/* Cast-wrapping multiplication: (unsigned long)(a * b) */
void cast_wrap_multiply(void) {
    int a = 50000;
    int b = 50000;
    unsigned long result;
    result = (unsigned long)(a * b);
    (void)result;
}

/* Cast one operand then multiply: (unsigned long)a * b */
void cast_operand_multiply(void) {
    int a = 50000;
    int b = 50000;
    unsigned long result;
    result = (unsigned long)a * b;
    (void)result;
}
