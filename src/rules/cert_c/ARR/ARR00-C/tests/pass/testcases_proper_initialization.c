/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR00-C violation
 */

#include <stdio.h>
#include <string.h>

int main() {
    int arr1[5] = {1, 2, 3, 4, 5};

    int arr2[10] = {0};

    int arr3[] = {10, 20, 30, 40, 50};
    size_t arr3_size = sizeof(arr3) / sizeof(arr3[0]);

    char str[100] = "Hello, World!";

    float matrix[3][3] = {
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };

    printf("arr3 has %zu elements\n", arr3_size);
    printf("String: %s\n", str);

    return 0;
}