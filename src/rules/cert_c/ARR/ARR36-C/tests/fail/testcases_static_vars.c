/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers to different static variables
 */

#include <stddef.h>

void use_static(void) {
    static int var1 = 10;
    static int var2 = 20;
    static int var3 = 30;

    int *ptr1 = &var1;
    int *ptr2 = &var3;

    // Subtract pointers to different static variables
    ptrdiff_t diff = ptr2 - ptr1;  // Line 18 - VIOLATION
}

int main(void) {
    use_static();
    return 0;
}
