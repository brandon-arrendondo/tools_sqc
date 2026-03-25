/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: FAIL - Pointer parameters should be declared const when not modified
 */

#include <stdio.h>
#include <string.h>

/* Simple read-only pointer not declared const */
int sum_array(int *arr, int n) {
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += arr[i];
    }
    return total;
}

/* Pointer parameter only read through dereference */
void print_value(int *ptr) {
    printf("value = %d\n", *ptr);
}

/* Multiple params, only second is read-only */
void copy_data(char *dest, char *src, int n) {
    memcpy(dest, src, n);
}

/* Struct pointer only read */
struct Point { int x; int y; };
int get_distance(struct Point *p) {
    return p->x + p->y;
}
