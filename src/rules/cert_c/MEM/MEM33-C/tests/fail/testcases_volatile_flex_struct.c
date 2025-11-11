/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: volatile_flex_struct.c
 *
 * This case demonstrates a violation of MEM33-C by using volatile-qualified
 * structures with flexible array members in automatic storage, which
 * compounds the violation with additional undefined behavior.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    /* VIOLATION: volatile flexible array structure in automatic storage */
    volatile struct flex_array_struct volatile_flex;

    volatile_flex.num = 4;

    /* VIOLATION: Accessing flexible array member of volatile struct in automatic storage */
    volatile_flex.data[0] = 11;  /* Undefined behavior */
    volatile_flex.data[1] = 22;  /* Undefined behavior */
    volatile_flex.data[2] = 33;  /* Undefined behavior */
    volatile_flex.data[3] = 44;  /* Undefined behavior */

    printf("Volatile struct num: %zu\n", volatile_flex.num);

    /* Reading also causes undefined behavior */
    printf("Volatile data: ");
    for (size_t i = 0; i < volatile_flex.num; i++) {
        printf("%d ", volatile_flex.data[i]);
    }
    printf("\n");

    /* Compound violation: casting away volatile and still wrong */
    struct flex_array_struct *non_volatile =
        (struct flex_array_struct *)&volatile_flex;
    non_volatile->data[0] = 99;  /* Still undefined behavior due to automatic storage */

    printf("Modified through cast: %d\n", non_volatile->data[0]);

    return 0;
}