/*
 * Rule: DCL30-C
 * Source: testcases
 * Status: FAIL - Return or assign pointer to local storage
 */

/* Return pointer to local array */
char *return_local_array(void) {
    char buf[256];
    buf[0] = 'A';
    return buf;
}

/* Return address of local variable */
int *return_local_addr(void) {
    int x = 42;
    return &x;
}

/* Return local struct */
struct Data { int x; };
struct Data *return_local_struct(void) {
    struct Data d;
    d.x = 1;
    return &d;
}
