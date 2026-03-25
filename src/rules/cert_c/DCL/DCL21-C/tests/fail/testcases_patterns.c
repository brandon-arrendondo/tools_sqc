/* Rule: DCL21-C
 * Source: testcases
 * Status: FAIL - Address of compound literal taken inside a loop
 */

#include <stdio.h>

typedef struct point {
    int x;
    int y;
} point;

/* Case 1: Address of compound literal in for loop */
void test_for_loop(void) {
    point *points[5];
    for (int i = 0; i < 5; i++) {
        points[i] = &(point){i, i * 2};
    }
}

/* Case 2: Address of compound literal in while loop */
void test_while_loop(void) {
    int *ptrs[3];
    int idx = 0;
    while (idx < 3) {
        ptrs[idx] = &(int){idx * 10};
        idx++;
    }
}

/* Case 3: Address of compound literal in do-while loop */
void test_do_while_loop(void) {
    point *p;
    int count = 0;
    do {
        p = &(point){count, count + 1};
        printf("%d %d\n", p->x, p->y);
        count++;
    } while (count < 3);
}
