/*
 * Rule: INT07-C
 * Source: testcases
 * Status: PASS - Explicit signed/unsigned char used correctly
 */

/* unsigned char for numeric value */
void unsigned_char_arithmetic(unsigned char uc) {
    int result = uc + 1;
    (void)result;
}

/* signed char for explicitly signed value */
void signed_char_arithmetic(signed char sc) {
    int result = sc + 1;
    (void)result;
}

/* Plain char for character data (non-numeric) — acceptable */
void char_for_text(void) {
    char letter = 'A';
    char next = letter;
    (void)next;
}
