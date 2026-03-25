/*
 * Rule: FLP00-C
 * Source: wiki
 * Status: FAIL - Direct equality comparison of floating-point values
 */

#include <math.h>

int check_result(void) {
    float c = 3.14f;
    if (c == 3.14f) {
        return 1;
    }
    return 0;
}
