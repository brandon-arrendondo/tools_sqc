/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Direct member access instead of pointer arithmetic
 */

struct numbers {
    short num_a, num_b, num_c;
};

int sum_numbers(const struct numbers *numb) {
    // Direct member access - COMPLIANT
    int total = numb->num_a + numb->num_b + numb->num_c;
    return total;
}

int main(void) {
    struct numbers n = {10, 20, 30};
    int result = sum_numbers(&n);
    return 0;
}
