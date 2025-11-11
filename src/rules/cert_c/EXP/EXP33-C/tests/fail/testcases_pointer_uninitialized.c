/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: pointer_uninitialized.c
 *
 * Multiple violations involving uninitialized pointers
 */

#include <stdio.h>
#include <stdlib.h>

/* NON-COMPLIANT: Uninitialized pointer dereference */
void unsafe_pointer_deref(void) {
    int *ptr;  /* Uninitialized pointer */

    if (rand() % 2) {
        ptr = malloc(sizeof(int));
        *ptr = 42;
    }

    printf("Value: %d\n", *ptr);  /* May dereference uninitialized pointer */
    free(ptr);  /* May free invalid pointer */
}

/* NON-COMPLIANT: Uninitialized function pointer */
void unsafe_function_pointer(void) {
    int (*func_ptr)(int);  /* Uninitialized function pointer */

    if (rand() % 2) {
        func_ptr = abs;
    }

    int result = func_ptr(-5);  /* May call through uninitialized pointer */
    printf("Result: %d\n", result);
}

/* NON-COMPLIANT: Array of uninitialized pointers */
void unsafe_pointer_array(void) {
    char *strings[5];  /* Array of uninitialized pointers */

    strings[0] = "Hello";
    /* strings[1-4] uninitialized */

    for (int i = 0; i < 5; i++) {
        printf("String %d: %s\n", i, strings[i]);  /* May access uninitialized pointers */
    }
}

int main(void) {
    unsafe_pointer_deref();
    unsafe_function_pointer();
    unsafe_pointer_array();
    return 0;
}