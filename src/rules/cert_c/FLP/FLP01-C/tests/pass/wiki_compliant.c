/*
 * Rule: FLP01-C
 * Source: wiki
 * Status: PASS - Conservative stub rule (CERT "unenforceable"), never flags
 */

#include <math.h>

float compute(float x, float y, float z) {
    /* These rearrangements may affect precision but are not
       detectable by automated analysis per CERT guidance */
    float result = x * y * z;
    result = (x - y) + y;
    result = x + x * y;
    return result;
}
