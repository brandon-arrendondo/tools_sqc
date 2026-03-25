/*
 * Rule: DCL30-C
 * Source: testcases
 * Status: FAIL - Assigning local storage to global pointers
 */

int *g_ptr;

/* Assign local array to global */
void assign_local_to_global(void) {
    int arr[10];
    arr[0] = 1;
    g_ptr = arr;
}

/* Assign address of local to global */
void assign_addr_to_global(void) {
    int x = 42;
    g_ptr = &x;
}
