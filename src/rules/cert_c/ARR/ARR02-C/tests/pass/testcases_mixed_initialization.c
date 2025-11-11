/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

typedef struct {
    int id;
    char name[50];
    double score;
} student_t;

int main() {
    student_t students[20] = {
        {1, "Alice", 95.5},
        {2, "Bob", 87.3},
        [5] = {6, "Charlie", 92.1},
        [19] = {20, "Zoe", 88.8}
    };

    int mixed_data[30] = {
        1, 2, 3, 4, 5,
        [10] = 100,
        [20] = 200,
        [25] = 250, 251, 252
    };

    char text_buffer[256] = "Initial text";

    float measurements[50] = {1.1, 2.2, [49] = 99.9};

    printf("Mixed initialization with explicit bounds\n");

    return 0;
}