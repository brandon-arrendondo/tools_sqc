/*
 * Rule: STR09-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR09-C violation
 * Description: Digit ordering and equality comparisons are portable
 */

int is_digit(char c) {
    return (c >= '0') && (c <= '9');  /* Safe: digits are contiguous */
}

int is_vowel(char c) {
    return (c == 'a') || (c == 'e') || (c == 'i') ||
           (c == 'o') || (c == 'u');  /* Safe: equality only */
}
