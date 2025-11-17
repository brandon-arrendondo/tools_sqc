/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Function pointer array accessed beyond allocated bounds
 */

#include <stdio.h>

void func1(void) { printf("Function 1\n"); }
void func2(void) { printf("Function 2\n"); }
void func3(void) { printf("Function 3\n"); }

int main(void) {
    void (*functions[3])(void) = {func1, func2, func3};
    int choice = 5;

    // Access function pointer array beyond bounds
    if (functions[choice] != NULL) {
        functions[choice]();  // May crash or call random function
    }

    // Assignment beyond bounds
    functions[7] = func1;

    return 0;
}