/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Typedef array type accessed beyond defined bounds
 */

#include <stdio.h>

typedef int IntArray[7];
typedef struct {
    IntArray numbers;
    char name[20];
} NumberSet;

int main(void) {
    NumberSet set = {{10, 20, 30, 40, 50, 60, 70}, "MyNumbers"};

    // Access beyond typedef array bounds
    printf("numbers[10] = %d\n", set.numbers[10]);
    set.numbers[12] = 999;

    // Direct typedef access
    IntArray local_array = {1, 2, 3, 4, 5, 6, 7};
    printf("local_array[8] = %d\n", local_array[8]);

    return 0;
}