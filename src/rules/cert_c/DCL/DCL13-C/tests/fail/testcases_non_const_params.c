/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: FAIL - Non-const pointer params that are not modified
 */

#include <stdio.h>

/* Pointer param only read, not modified */
int sum_array(int *arr, int len) {
    int total = 0;
    for (int i = 0; i < len; i++) {
        total += arr[i];
    }
    return total;
}

/* Pointer param only used in printf (read-only function) */
void print_string(char *str) {
    printf("%s\n", str);
}

/* Pointer param used with strlen (read-only) */
int get_length(char *s) {
    return (int)strlen(s);
}

/* Pointer param only used for reading struct fields */
struct Point { int x; int y; };
int get_x(struct Point *p) {
    return p->x;
}

/* Multiple pointer params, none modified */
int compare_values(int *a, int *b) {
    return *a - *b;
}

/* Array parameter not modified */
int first_element(int arr[]) {
    return arr[0];
}
