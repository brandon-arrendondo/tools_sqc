/*
 * Rule: DCL18-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL18-C violation
 * Description: Floating point with leading zero is not octal
 */

void float_constants(void) {
    double a = 0.5;      /* Float, not octal */
    double b = 0.001;    /* Float, not octal */
    float c = 0.0f;      /* Float zero */
    double d = 0e10;     /* Scientific notation */
}
