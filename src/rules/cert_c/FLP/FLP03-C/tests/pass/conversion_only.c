/*
 * Rule: FLP03-C
 * Source: testcases
 * Status: PASS - Cast to float/double without division should not flag.
 *         v0.3.44 removed overbroad check_fp_conversion().
 */

float int_to_float(int x) {
    return (float)x;
}

double int_to_double(int x) {
    return (double)x;
}

double float_to_double(float f) {
    return (double)f;
}
