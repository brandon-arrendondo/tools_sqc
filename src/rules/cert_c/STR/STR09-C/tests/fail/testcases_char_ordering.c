/*
 * Rule: STR09-C
 * Source: testcases
 * Status: FAIL - Should trigger STR09-C violation
 * Description: Ordering comparison on non-digit char is non-portable
 */

void testcase_noncompliant_letter_range(void) {
    char ch = 'b';
    if ((ch >= 'a') && (ch <= 'z')) {  /* Violation: letter ordering assumed */
        /* ... */
    }
}
