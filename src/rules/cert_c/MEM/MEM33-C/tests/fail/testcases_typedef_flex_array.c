/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: typedef_flex_array.c
 *
 * This case demonstrates a violation of MEM33-C by using typedef with
 * flexible array structures and then incorrectly using them in automatic
 * storage or with improper memory management.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

typedef struct flex_array_struct FlexType;

int main(void) {
    /* VIOLATION: Using typedef'd flexible array structure in automatic storage */
    FlexType local_typedef;

    local_typedef.num = 2;

    /* VIOLATION: Accessing flexible array through typedef */
    local_typedef.data[0] = 10;  /* Undefined behavior */
    local_typedef.data[1] = 20;  /* Undefined behavior */

    printf("Typedef struct num: %zu\n", local_typedef.num);
    printf("Typedef data: %d, %d\n", local_typedef.data[0], local_typedef.data[1]);

    /* Another violation: incorrect allocation using typedef */
    FlexType *ptr = malloc(sizeof(FlexType));  /* VIOLATION: Wrong size */
    if (ptr == NULL) return 1;

    ptr->num = 3;
    ptr->data[0] = 100;  /* Buffer overflow */
    ptr->data[1] = 200;  /* Buffer overflow */
    ptr->data[2] = 300;  /* Buffer overflow */

    printf("Dynamically allocated but wrong size:\n");
    for (size_t i = 0; i < ptr->num; i++) {
        printf("data[%zu] = %d\n", i, ptr->data[i]);
    }

    free(ptr);
    return 0;
}