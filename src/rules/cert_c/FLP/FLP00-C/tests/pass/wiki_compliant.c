/*
 * Rule: FLP00-C
 * Source: wiki
 * Status: PASS - Epsilon-based floating-point comparison
 */

#include <math.h>
#include <float.h>

int check_result(float a, float b) {
    float c = a / b;
    float diff = fabsf(c - (a / b));
    if (diff <= FLT_EPSILON) {
        return 1;
    }
    return 0;
}
