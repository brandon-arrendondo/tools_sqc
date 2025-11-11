/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

int main() {
    const int primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

    const char hex_digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    const double constants[5] = {
        3.14159,    // PI
        2.71828,    // E
        1.41421     // sqrt(2)
    };

    const int lookup_table[8][8] = {
        {0, 1, 2, 3, 4, 5, 6, 7},
        {1, 2, 3, 4, 5, 6, 7, 8}
    };

    printf("Const arrays with explicit bounds\n");

    return 0;
}