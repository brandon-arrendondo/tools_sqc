/* EXP33-C: Partial array initialization — should flag.
 * Arrays allocated via malloc/alloca where only a fraction of elements
 * are initialized before all elements are read. */

#include <stdlib.h>

/* Partial init via alloca */
void bad_alloca_partial_init(void) {
    int *data;
    data = (int *)__builtin_alloca(10 * sizeof(int));
    /* Only initialize first 5 of 10 elements */
    int i;
    for (i = 0; i < (10 / 2); i++) {
        data[i] = i;
    }
    /* FLAW: Reading all 10 elements — elements 5-9 are uninitialized */
    for (i = 0; i < 10; i++) {
        (void)data[i];
    }
}

/* Partial init via malloc */
void bad_malloc_partial_init(void) {
    int *data;
    data = (int *)malloc(10 * sizeof(int));
    /* Only initialize first 5 of 10 elements */
    int i;
    for (i = 0; i < 5; i++) {
        data[i] = i;
    }
    /* FLAW: Reading all 10 elements — elements 5-9 are uninitialized */
    for (i = 0; i < 10; i++) {
        (void)data[i];
    }
    free(data);
}
