/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Array member in struct accessed beyond bounds
 */

#include <stdio.h>

typedef struct {
    int id;
    char name[10];
    int scores[5];
} Student;

int main(void) {
    Student student = {1, "John", {85, 90, 78, 92, 88}};

    // Access beyond name array bounds
    student.name[15] = 'X';

    // Access beyond scores array bounds
    printf("Score[10] = %d\n", student.scores[10]);
    student.scores[8] = 100;

    return 0;
}