/*
 * Rule: BRULE-065
 * Status: FAIL - Triple pointer exceeds max depth of 2
 */

void test_triple_pointer(void) {
    int ***p = 0;
}

void test_triple_pointer_param(int ***p) {
    (void)p;
}

void test_quad_pointer(void) {
    char ****q = 0;
}
