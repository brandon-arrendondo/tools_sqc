/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: array_of_flex_structs.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to create
 * an array of structures containing flexible array members. Arrays of
 * flexible array structures cannot be properly allocated in contiguous memory.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    /* VIOLATION: Attempting to create array of flexible array structures */
    struct flex_array_struct flex_array[3];

    /* Initialize each structure */
    for (int i = 0; i < 3; i++) {
        flex_array[i].num = i + 1;

        /* VIOLATION: Accessing flexible array members without proper allocation */
        for (size_t j = 0; j < flex_array[i].num; j++) {
            flex_array[i].data[j] = (int)(i * 10 + j);  /* Undefined behavior */
        }
    }

    /* Accessing the data results in undefined behavior */
    printf("Array of flexible structures:\n");
    for (int i = 0; i < 3; i++) {
        printf("Struct %d (num=%zu): ", i, flex_array[i].num);
        for (size_t j = 0; j < flex_array[i].num; j++) {
            printf("%d ", flex_array[i].data[j]);
        }
        printf("\n");
    }

    return 0;
}