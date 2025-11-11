/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: direct_assignment.c
 *
 * This case demonstrates a violation of MEM33-C by using direct assignment
 * to copy a structure containing a flexible array member. The rule requires
 * using memcpy() with the proper size calculation instead of direct assignment.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *src, *dest;
    size_t array_size = 10;

    /* Proper allocation */
    src = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (src == NULL) return 1;

    dest = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (dest == NULL) {
        free(src);
        return 1;
    }

    src->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        src->data[i] = (int)i;
    }

    /* VIOLATION: Direct assignment doesn't copy the flexible array data */
    *dest = *src;  /* Only copies fixed members, not flexible array */

    /* dest->data now contains garbage values */
    printf("dest->data[0] = %d\n", dest->data[0]);

    free(src);
    free(dest);
    return 0;
}