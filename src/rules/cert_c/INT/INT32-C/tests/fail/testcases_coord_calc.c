/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Coordinate calculation can overflow when scaling or transforming coordinates
 */

#include <limits.h>
#include <stdio.h>

typedef struct {
    int x, y;
} Point;

Point scale_point(Point p, int scale_factor) {
    Point result;
    // Extract to locals so INT32-C can resolve types
    // (field_expression types can't be resolved without struct definitions)
    int px = p.x;
    int py = p.y;
    // VIOLATION: multiplication can overflow
    result.x = px * scale_factor;
    result.y = py * scale_factor;
    return result;
}

int main() {
    Point original = {1000000, 1000000};
    int scale = 3000;

    Point scaled = scale_point(original, scale);

    printf("Original: (%d, %d)\n", original.x, original.y);
    printf("Scaled: (%d, %d)\n", scaled.x, scaled.y);

    return 0;
}