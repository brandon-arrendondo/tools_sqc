/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: PASS - Pointer parameters correctly declared const or modified
 */

#include <stdio.h>
#include <string.h>

/* Already const — compliant */
int sum_array(const int *arr, int n) {
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += arr[i];
    }
    return total;
}

/* Parameter IS modified — no const needed */
void zero_array(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = 0;
    }
}

/* Parameter modified through dereference */
void set_value(int *ptr) {
    *ptr = 42;
}

/* Struct pointer modified via arrow */
struct Point { int x; int y; };
void reset_point(struct Point *p) {
    p->x = 0;
    p->y = 0;
}
