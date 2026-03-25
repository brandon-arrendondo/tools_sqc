/* Rule: DCL21-C
 * Source: testcases
 * Status: PASS - Compound literals used safely without address-of in loops
 */

#include <stdio.h>

typedef struct point {
    int x;
    int y;
} point;

/* Case 1: Copy compound literal by value in loop (safe) */
void test_copy_by_value(void) {
    point points[5];
    for (int i = 0; i < 5; i++) {
        points[i] = (point){i, i * 2};
    }
}

/* Case 2: Use regular variables instead of compound literal address */
void test_regular_variables(void) {
    point storage[3];
    point *ptrs[3];
    for (int i = 0; i < 3; i++) {
        storage[i].x = i;
        storage[i].y = i + 1;
        ptrs[i] = &storage[i];
    }
}

/* Case 3: Address of compound literal outside loop (safe - no loop scope issue) */
void test_outside_loop(void) {
    point *p = &(point){10, 20};
    printf("%d %d\n", p->x, p->y);
}

/* Case 4: Compound literal without address-of in while loop (safe) */
void test_value_in_while(void) {
    int idx = 0;
    while (idx < 3) {
        point p = (point){idx, idx * 2};
        printf("%d %d\n", p.x, p.y);
        idx++;
    }
}
