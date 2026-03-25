/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: FAIL - Nested access patterns that are still read-only
 */

/* Nested struct access — read only */
struct Inner { int val; };
struct Outer { struct Inner inner; };
int read_nested(struct Outer *o) {
    return o->inner.val;
}

/* Array subscript read-only */
int read_subscript(int *arr, int idx) {
    return arr[idx];
}

/* Pointer dereference read-only */
int read_deref(int *p) {
    return *p;
}

/* Passed only to read-only function (strcmp) */
int compare_strings(char *a, char *b) {
    return strcmp(a, b);
}
