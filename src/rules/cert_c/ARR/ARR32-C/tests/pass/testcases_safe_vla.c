/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: PASS - VLA with validated size or fixed-size array
 */

/* Fixed-size array — no VLA issue */
void fixed_array(void) {
    int arr[100];
    arr[0] = 42;
    (void)arr;
}

/* VLA with size validation */
void validated_vla(int n) {
    if (n > 0 && n <= 1024) {
        int arr[n];
        arr[0] = 42;
        (void)arr;
    }
}
