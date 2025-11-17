/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR02-C violation
 */

#include <stdio.h>

int main() {
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    double values[] = {1.1, 2.2, 3.3, 4.4, 5.5};

    char vowels[] = {'a', 'e', 'i', 'o', 'u'};

    float matrix[][4] = {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {9.0, 10.0, 11.0, 12.0}
    };

    printf("Arrays with implicit bounds\n");

    return 0;
}