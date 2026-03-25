/*
 * Rule: FLP00-C
 * Source: testcases
 * Status: FAIL - Float equality comparisons in conditions
 */

/* Float literal equality in if-condition */
int check_float(float x) {
    if (x == 1.0f) {
        return 1;
    }
    return 0;
}

/* Double literal equality in if-condition */
int check_double(double x) {
    if (x == 3.14) {
        return 1;
    }
    return 0;
}

/* Float != literal in if-condition */
int check_not_equal(float x) {
    if (x != 0.0f) {
        return 1;
    }
    return 0;
}
