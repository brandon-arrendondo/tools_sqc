/*
 * Rule: STR34-C
 * Source: testcases
 * Status: PASS - Proper unsigned char cast before widening
 */

/* Correct: cast to unsigned char before widening */
void safe_init(const char *str) {
    int val = (unsigned char)*str;
    (void)val;
}

/* Already unsigned char — no cast needed */
void unsigned_char_source(const unsigned char *str) {
    int val = *str;
    (void)val;
}
