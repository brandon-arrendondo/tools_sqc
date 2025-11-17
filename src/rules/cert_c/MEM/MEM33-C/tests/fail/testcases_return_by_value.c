/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: return_by_value.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to return
 * a structure containing a flexible array member by value from a function.
 * This only returns the fixed members and loses the flexible array data.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* VIOLATION: Function returns flexible array struct by value */
struct flex_array_struct create_flex_struct(size_t size) {
    struct flex_array_struct *temp;
    temp = malloc(sizeof(struct flex_array_struct) + sizeof(int) * size);
    if (temp == NULL) {
        /* Return empty struct on allocation failure */
        struct flex_array_struct empty = {0};
        return empty;
    }

    temp->num = size;
    for (size_t i = 0; i < size; i++) {
        temp->data[i] = (int)(i * 2);
    }

    /* VIOLATION: Returning by value only returns fixed members */
    struct flex_array_struct result = *temp;
    free(temp);
    return result;  /* Flexible array data is lost */
}

int main(void) {
    struct flex_array_struct flex_struct = create_flex_struct(5);

    printf("Number of elements: %zu\n", flex_struct.num);

    /* Accessing data results in undefined behavior */
    if (flex_struct.num > 0) {
        printf("First element: %d\n", flex_struct.data[0]);
    }

    return 0;
}