/*
 * Rule: DCL03-C
 * Source: testcases
 * Status: PASS - Runtime assert on non-constant expressions
 */

#include <assert.h>
#include <stdlib.h>

/* Assert on runtime value — cannot use static_assert */
void check_runtime(int x) {
    assert(x > 0);
}

/* Assert on pointer — runtime check */
void check_alloc(void) {
    int *p = (int *)malloc(sizeof(int));
    assert(p != NULL);
    free(p);
}
