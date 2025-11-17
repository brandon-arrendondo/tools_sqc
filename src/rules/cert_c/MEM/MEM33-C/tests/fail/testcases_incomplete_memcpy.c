/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: incomplete_memcpy.c
 *
 * This case demonstrates a violation of MEM33-C by using memcpy() with
 * incorrect size calculation, copying only the fixed members and not
 * the flexible array data.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *src, *dest;
    size_t array_size = 8;

    src = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (src == NULL) return 1;

    dest = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (dest == NULL) {
        free(src);
        return 1;
    }

    src->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        src->data[i] = (int)(i + 100);
    }

    /* VIOLATION: Incorrect size in memcpy - only copies fixed members */
    memcpy(dest, src, sizeof(struct flex_array_struct));

    /* dest->data contains garbage values */
    printf("Copied data[0]: %d (should be %d)\n", dest->data[0], src->data[0]);

    free(src);
    free(dest);
    return 0;
}