/*
 * Rule: STR09-C
 * Source: wiki
 * Status: FAIL - Should trigger STR09-C violation
 * Description: Ordering comparison on non-digit char is non-portable
 */

void testcase_noncompliant_char_ordering(void) {
    char ch = 'b';
    if ((ch >= 'a') && (ch <= 'c')) {  /* Violation: ordering assumed for letters */
        /* ... */
    }
}
