/*
 * Rule: API07-C
 * Source: testcases
 * Status: PASS - Safe type casts (same or smaller size)
 */

#include <stdlib.h>

/* Cast to same-size type */
void same_size_cast(void) {
    int i = 42;
    void *data = &i;
    int val = *((int *)data);
    (void)val;
}

/* Cast to smaller type */
void smaller_cast(void) {
    long l = 100;
    void *data = &l;
    int val = *((int *)data);
    (void)val;
}
