/*
 * Rule: STR09-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR09-C violation
 * Description: Equality comparison on char is portable
 */

void testcase_compliant_char_equality(void) {
    char ch = 't';
    if ((ch == 'a') || (ch == 'b') || (ch == 'c')) {  /* Compliant: equality only */
        /* ... */
    }
}
