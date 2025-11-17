/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

int main() {
    int numbers[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    double values[5] = {1.1, 2.2, 3.3};

    char buffer[256] = {0};

    float matrix[3][4] = {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0}
    };

    printf("Arrays declared with explicit bounds\n");

    return 0;
}