/*
 * Rule: API07-C
 * Source: testcases
 * Status: PASS - Safe type cast patterns through void*
 */

#include <stdlib.h>

/* Same-size cast through void* (int to int) */
void same_size_int(void) {
    int val = 42;
    void *data = &val;
    int result = *((int *)data);
    (void)result;
}

/* Same-size cast through void* (double to double) */
void same_size_double(void) {
    double val = 3.14;
    void *data = &val;
    double result = *((double *)data);
    (void)result;
}

/* No void pointers used */
void no_void_ptrs(void) {
    int *p = (int *)malloc(sizeof(int));
    *p = 10;
    free(p);
}

/* void* without dereference */
void void_ptr_no_deref(void) {
    int val = 42;
    void *data = &val;
    (void)data;
}
