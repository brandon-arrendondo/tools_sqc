/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

int main() {
    int sparse_array[20] = {[0] = 1, [5] = 42, [10] = 100, [19] = 999};

    char vowels[26] = {['a'-'a'] = 1, ['e'-'a'] = 1, ['i'-'a'] = 1, ['o'-'a'] = 1, ['u'-'a'] = 1};

    int config[100] = {
        [0] = 10,
        [1] = 20,
        [50] = 500,
        [99] = 999
    };

    double coordinates[3][3] = {
        [0][0] = 1.0, [0][1] = 0.0, [0][2] = 0.0,
        [1][1] = 1.0,
        [2][2] = 1.0
    };

    printf("Arrays with explicit bounds and designated initializers\n");

    return 0;
}