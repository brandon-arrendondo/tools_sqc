/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: function_parameter_by_value.c
 *
 * This case demonstrates a violation of MEM33-C by defining function
 * parameters that accept structures with flexible array members by value,
 * which cannot properly pass the flexible array data.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* VIOLATION: Function parameter accepts flexible array struct by value */
void print_flex_struct(struct flex_array_struct flex) {
    printf("Function received struct with num: %zu\n", flex.num);

    /* VIOLATION: Accessing flexible array data passed by value */
    if (flex.num > 0) {
        printf("First element: %d\n", flex.data[0]);  /* Undefined behavior */
    }

    /* Attempting to access more elements */
    for (size_t i = 0; i < flex.num && i < 3; i++) {
        printf("data[%zu] = %d\n", i, flex.data[i]);  /* Undefined behavior */
    }
}

/* VIOLATION: Another function with by-value parameter */
struct flex_array_struct modify_flex_struct(struct flex_array_struct input) {
    input.num = 1;
    input.data[0] = 999;  /* Undefined behavior */
    return input;  /* Only returns fixed members */
}

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t array_size = 3;

    /* Proper allocation */
    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (flex_struct == NULL) return 1;

    flex_struct->num = array_size;
    flex_struct->data[0] = 100;
    flex_struct->data[1] = 200;
    flex_struct->data[2] = 300;

    /* VIOLATION: Passing flexible array struct by value */
    print_flex_struct(*flex_struct);

    /* VIOLATION: Another by-value call */
    struct flex_array_struct result = modify_flex_struct(*flex_struct);
    printf("Returned struct num: %zu\n", result.num);

    free(flex_struct);
    return 0;
}