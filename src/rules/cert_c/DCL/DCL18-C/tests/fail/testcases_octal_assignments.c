/*
 * Rule: DCL18-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL18-C violation
 * Description: Octal constants in variable assignments
 */

void use_octal_by_mistake(void) {
    int port = 0080;     /* Violation: invalid octal (8 not octal digit) */
    int count = 0010;    /* Violation: octal 8, not decimal 10 */
    int offset = 0100;   /* Violation: octal 64, not decimal 100 */
}
