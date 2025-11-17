/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

struct point {
    double x, y, z;
};

struct data_set {
    int values[10];
    char labels[5][20];
};

int main() {
    struct point vertices[4] = {
        {0.0, 0.0, 0.0},
        {1.0, 0.0, 0.0},
        {1.0, 1.0, 0.0}
    };

    struct data_set dataset[2] = {
        {
            .values = {1, 2, 3, 4, 5},
            .labels = {"first", "second", "third"}
        },
        {
            .values = {10, 20, 30}
        }
    };

    int status_codes[50] = {
        [0] = 200,  // OK
        [1] = 404,  // Not Found
        [2] = 500   // Internal Server Error
    };

    printf("Aggregate types with explicit array bounds\n");

    return 0;
}