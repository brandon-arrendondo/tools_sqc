/*
 * Rule: BRULE-065
 * Status: PASS - Single and double pointers are within limits
 */

void test_single_pointer(void) {
    int *a = 0;
    char *b = 0;
}

void test_double_pointer(void) {
    int **p = 0;
    char **argv = 0;
}

void test_param_single(int *a) {
    (void)a;
}

void test_param_double(int **a) {
    (void)a;
}

void test_no_pointer(void) {
    int x = 0;
    (void)x;
}
