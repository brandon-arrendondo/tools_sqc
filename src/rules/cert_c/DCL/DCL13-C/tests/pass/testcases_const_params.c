/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: PASS - Already const-qualified pointer params
 */

#include <stdio.h>
#include <string.h>

/* Already const pointer */
int sum_const(const int *arr, int len) {
    int total = 0;
    for (int i = 0; i < len; i++) {
        total += arr[i];
    }
    return total;
}

/* Const char* parameter */
void print_const(const char *msg) {
    printf("%s\n", msg);
}

/* Multiple const params */
int compare_const(const int *a, const int *b) {
    return *a - *b;
}

/* Const with struct pointer */
struct Point { int x; int y; };
int get_point_x(const struct Point *p) {
    return p->x;
}

/* Non-pointer param — not checked */
int double_value(int x) {
    return x * 2;
}
