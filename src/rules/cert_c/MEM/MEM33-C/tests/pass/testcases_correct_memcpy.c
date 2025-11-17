/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: correct_memcpy.c
 *
 * This case demonstrates compliant code that properly copies structures
 * containing flexible array members using memcpy() with correct size
 * calculation instead of direct assignment.
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
    size_t array_size = 6;
    size_t total_size;

    /* COMPLIANT: Proper dynamic allocation */
    total_size = sizeof(struct flex_array_struct) + sizeof(int) * array_size;

    src = malloc(total_size);
    if (src == NULL) return 1;

    dest = malloc(total_size);
    if (dest == NULL) {
        free(src);
        return 1;
    }

    /* Initialize source structure */
    src->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        src->data[i] = (int)(i + 100);
    }

    /* COMPLIANT: Proper copying using memcpy with correct size */
    memcpy(dest, src, total_size);

    /* Verify the copy */
    printf("Source and destination comparison:\n");
    printf("Source num: %zu, Dest num: %zu\n", src->num, dest->num);

    printf("Data comparison:\n");
    for (size_t i = 0; i < array_size; i++) {
        printf("src[%zu] = %d, dest[%zu] = %d\n",
               i, src->data[i], i, dest->data[i]);
    }

    /* Verify they are equal */
    int equal = (memcmp(src, dest, total_size) == 0);
    printf("Structures are %s\n", equal ? "identical" : "different");

    /* COMPLIANT: Proper cleanup */
    free(src);
    free(dest);
    return 0;
}