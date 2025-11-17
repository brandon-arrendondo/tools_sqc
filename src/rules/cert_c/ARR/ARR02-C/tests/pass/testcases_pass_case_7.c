/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

int main() {
    int numbers[10] = {1, 2, 3, 4, 5};
    double values[5] = {1.1, 2.2, 3.3};
    char buffer[256] = {0};
    
    printf("Arrays with explicit bounds\n");
    return 0;
}
