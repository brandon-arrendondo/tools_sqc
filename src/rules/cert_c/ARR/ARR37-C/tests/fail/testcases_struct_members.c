/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on struct members assuming contiguity
 */

struct numbers {
    short num_a, num_b, num_c;
};

int sum_numbers(const struct numbers *numb) {
    int total = 0;
    const short *numb_ptr;

    // Iterate over struct members with pointer arithmetic
    for (numb_ptr = &numb->num_a;
         numb_ptr <= &numb->num_c;
         numb_ptr++) {  // Line 17 - VIOLATION
        total += *(numb_ptr);
    }
    return total;
}

int main(void) {
    struct numbers n = {1, 2, 3};
    sum_numbers(&n);
    return 0;
}
