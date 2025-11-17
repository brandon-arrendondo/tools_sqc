/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: typedef_uninitialized.c
 */

#include <stdio.h>

typedef struct {
    int x, y;
} Point;

typedef int (*MathFunc)(int, int);

/* NON-COMPLIANT: Typedef-based types uninitialized */
void unsafe_typedef_usage(void) {
    Point p;         /* Uninitialized typedef struct */
    MathFunc func;   /* Uninitialized function pointer typedef */

    printf("Point: (%d, %d)\n", p.x, p.y);  /* Reading uninitialized */

    if (func != NULL) {  /* Comparing uninitialized function pointer */
        int result = func(1, 2);
        printf("Result: %d\n", result);
    }
}

int main(void) {
    unsafe_typedef_usage();
    return 0;
}