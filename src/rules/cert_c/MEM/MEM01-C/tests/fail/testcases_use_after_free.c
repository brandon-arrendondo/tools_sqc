/*
 * Rule: MEM01-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM01-C violation
 * Description: Use-after-free patterns detected via CFG reachability
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void deref_after_free(void) {
    char *p = malloc(100);
    free(p);  /* Violation: p is dereferenced below */
    *p = 'A';
}

void pass_to_function_after_free(void) {
    char *p = malloc(100);
    if (p == NULL) return;
    strcpy(p, "hello");
    free(p);  /* Violation: p is passed to printf below */
    printf("%s\n", p);
}

void subscript_after_free(void) {
    int *arr = malloc(10 * sizeof(int));
    free(arr);  /* Violation: arr is subscripted below */
    arr[0] = 42;
}

char *return_after_free(void) {
    /* Note: returning freed pointer is a use */
    char *p = malloc(100);
    free(p);  /* Violation: p is returned below */
    return p;
}
