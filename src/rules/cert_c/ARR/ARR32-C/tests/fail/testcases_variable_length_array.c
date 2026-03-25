/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Variable-length array with unvalidated size
 */

/* VLA with unchecked size parameter */
void create_buffer(int n) {
    int arr[n];
    arr[0] = 42;
    (void)arr;
}

/* VLA from function return */
int get_size(void);
void vla_from_func(void) {
    int n = get_size();
    char buf[n];
    buf[0] = 'x';
    (void)buf;
}
