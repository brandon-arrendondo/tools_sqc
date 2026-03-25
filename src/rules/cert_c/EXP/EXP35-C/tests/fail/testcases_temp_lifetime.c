/*
 * Rule: EXP35-C
 * Source: testcases
 * Status: FAIL - Modifying or accessing objects with temporary lifetime
 */

#include <stdio.h>

struct Data { int arr[4]; };

struct Data get_data(void) {
    struct Data d = { {1, 2, 3, 4} };
    return d;
}

/* Incrementing element of temporary struct array */
void modify_temporary_element(void) {
    ++get_data().arr[0];
}

/* Assigning pointer to temporary struct array member */
void pointer_to_temporary(void) {
    int *p = get_data().arr;
    (void)p;
}

/* Taking address of temporary array member */
void address_of_temporary(void) {
    int *p;
    p = &get_data().arr[0];
    (void)p;
}

/* Accessing temporary array in printf (C99 violation) */
struct Msg { char text[32]; };
struct Msg get_msg(void) {
    struct Msg m = { "hello" };
    return m;
}
void print_temporary(void) {
    printf("%s\n", get_msg().text);
}
