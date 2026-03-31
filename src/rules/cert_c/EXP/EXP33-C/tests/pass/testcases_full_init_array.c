/* EXP33-C: Full array initialization — should NOT flag.
 * Arrays allocated via malloc/alloca where all elements are initialized
 * before being read. */

#include <stdlib.h>

/* Full init via alloca */
void good_alloca_full_init(void) {
    int *data;
    data = (int *)__builtin_alloca(10 * sizeof(int));
    /* Initialize ALL 10 elements */
    int i;
    for (i = 0; i < 10; i++) {
        data[i] = i;
    }
    /* OK: All elements are initialized */
    for (i = 0; i < 10; i++) {
        (void)data[i];
    }
}

/* Full init via malloc */
void good_malloc_full_init(void) {
    int *data;
    data = (int *)malloc(10 * sizeof(int));
    /* Initialize ALL 10 elements */
    int i;
    for (i = 0; i < 10; i++) {
        data[i] = i;
    }
    /* OK: All elements are initialized */
    for (i = 0; i < 10; i++) {
        (void)data[i];
    }
    free(data);
}

/* Init via memset */
void good_memset_init(void) {
    int *data;
    data = (int *)malloc(10 * sizeof(int));
    __builtin_memset(data, 0, 10 * sizeof(int));
    /* OK: memset initializes all elements */
    int i;
    for (i = 0; i < 10; i++) {
        (void)data[i];
    }
    free(data);
}
