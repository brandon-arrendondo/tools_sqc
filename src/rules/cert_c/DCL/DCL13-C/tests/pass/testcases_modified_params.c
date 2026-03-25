/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: PASS - Pointer params that are properly modified
 */

#include <string.h>

/* Direct dereference write: *param = expr */
void set_value(int *out) {
    *out = 42;
}

/* Arrow write: param->field = expr */
struct Data { int value; };
void set_field(struct Data *d) {
    d->value = 10;
}

/* Subscript write: param[i] = expr */
void fill_array(int *arr, int len) {
    for (int i = 0; i < len; i++) {
        arr[i] = i;
    }
}

/* Compound assignment: param->field += expr */
void increment_field(struct Data *d) {
    d->value += 1;
}

/* Update expression: (*param)++ */
void inc_value(int *p) {
    (*p)++;
}

/* Update expression: param->field++ */
void inc_field(struct Data *d) {
    d->value++;
}

/* Passed to modifying function (unknown function) */
void helper(int *p);
void call_modifier(int *arr) {
    helper(arr);
}

/* Address-of member passed to unknown function */
void unknown_fn(int *p);
void pass_member_addr(struct Data *d) {
    unknown_fn(&d->value);
}

/* main() is always skipped */
int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return 0;
}
