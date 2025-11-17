/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof typedef'd type for scaling
 */

typedef struct {
    int x;
    int y;
    int z;
} point_t;

void typedef_sizeof(void) {
    point_t points[30];
    point_t *ptr = points;
    int offset = 5;

    // Manually scaling by sizeof(point_t)
    point_t *target = ptr + (offset * sizeof(point_t));  // Line 19 - VIOLATION
    target->x = 10;
}

int main(void) {
    typedef_sizeof();
    return 0;
}
