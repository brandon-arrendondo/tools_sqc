/*
 * Rule: DCL02-C
 * Source: testcases
 * Status: PASS - Identifiers that are visually distinct
 */

/* Same name in different function scopes — no conflict */
void func_a(void) {
    int count = 0;
    (void)count;
}
void func_b(void) {
    int count = 0;
    (void)count;
}

/* Single identifiers — nothing to confuse */
void single_id(void) {
    int alpha = 0;
    (void)alpha;
}

/* Clearly different identifiers */
void distinct_names(void) {
    int width = 10;
    int height = 20;
    int depth = 30;
    (void)width;
    (void)height;
    (void)depth;
}
