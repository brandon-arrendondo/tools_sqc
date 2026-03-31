/*
 * Rule: DCL18-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL18-C violation
 * Description: Leading zeros in array initializers create unintended octal
 */

int permissions[] = {
    0755,  /* Intended octal - but still flagged */
    0644,
    0100
};

int data[] = {
    0012,  /* Violation: looks decimal but is octal 10 */
    0034,  /* Violation: octal 28, not decimal 34 */
    0056   /* Violation: octal 46, not decimal 56 */
};
