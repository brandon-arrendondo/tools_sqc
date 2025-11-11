/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Enum values used as array indices without bounds validation
 */

#include <stdio.h>

typedef enum {
    RED = 0,
    GREEN = 1,
    BLUE = 2,
    INVALID = 10
} Color;

int main(void) {
    int color_values[3] = {255, 128, 64};
    Color color = INVALID;

    // Using enum value as array index without validation
    printf("Color value: %d\n", color_values[color]);
    color_values[color] = 999;

    // Another out-of-bounds enum
    Color bad_color = (Color)15;
    printf("Bad color value: %d\n", color_values[bad_color]);

    return 0;
}