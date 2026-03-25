/*
 * Rule: EXP35-C
 * Source: testcases
 * Status: PASS - Temporary objects stored before access
 */

#include <stdio.h>

struct Data { int arr[4]; };

struct Data get_data(void) {
    struct Data d = { {1, 2, 3, 4} };
    return d;
}

/* Store struct before accessing array member */
void safe_access_stored(void) {
    struct Data d = get_data();
    int val = d.arr[0];
    (void)val;
}

/* Store struct before modifying array member */
void safe_modify_stored(void) {
    struct Data d = get_data();
    d.arr[0]++;
}

/* Store struct before taking address */
void safe_address_stored(void) {
    struct Data d = get_data();
    int *p = &d.arr[0];
    (void)p;
}

/* Non-temporary struct access is always fine */
void local_struct_access(void) {
    struct Data d = { {10, 20, 30, 40} };
    int *p = d.arr;
    printf("%d\n", d.arr[1]);
    (void)p;
}

/* Returning a scalar from function is fine */
int get_value(void) { return 42; }
void scalar_return(void) {
    int v = get_value();
    (void)v;
}
