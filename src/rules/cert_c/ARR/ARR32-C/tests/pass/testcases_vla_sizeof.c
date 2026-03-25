/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - Fixed-size arrays and validated VLAs
 */

/* Fixed-size array (not a VLA) */
void fixed_array(void) {
    int arr[32];
    arr[0] = 1;
    (void)arr;
}

/* VLA with validated size */
void vla_validated(int n) {
    if (n > 0 && n <= 1024) {
        int arr[n];
        arr[0] = 1;
        (void)arr;
    }
}

/* Small compile-time constant */
void small_fixed(void) {
    char buf[256];
    buf[0] = 'A';
    (void)buf;
}
