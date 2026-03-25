/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - VLA with unchecked sizes
 */

/* VLA with function call size (unvalidated) */
int get_size(void);
void vla_from_function(void) {
    int n = get_size();
    int arr[n];
    arr[0] = 1;
    (void)arr;
}

/* VLA in loop with control variable */
void vla_in_loop(int n) {
    for (int i = 1; i <= n; i++) {
        int arr[i];
        arr[0] = i;
        (void)arr;
    }
}

/* VLA with bitwise shift (potential overflow) */
void vla_bitshift(int size) {
    int arr[size << 2];
    arr[0] = 1;
    (void)arr;
}
