/*
 * Rule: INT34-C
 * Source: testcases
 * Status: PASS - Shift amounts bounded by loop conditions
 */

/* for loop with i < 32 */
unsigned int shift_in_for_loop(unsigned int x) {
    unsigned int result = 0;
    for (int i = 0; i < 32; i++) {
        result |= (x << i);
    }
    return result;
}

/* for loop with 32 > i (reversed comparison) */
unsigned int shift_in_for_loop_reversed(unsigned int x) {
    unsigned int result = 0;
    for (int i = 0; 32 > i; i++) {
        result |= (x << i);
    }
    return result;
}

/* while loop with bound < 32 */
unsigned int shift_in_while(unsigned int x) {
    unsigned int result = 0;
    int i = 0;
    while (i < 32) {
        result |= (x << i);
        i++;
    }
    return result;
}

/* for loop with <= 31 */
unsigned int shift_in_for_loop_le(unsigned int x) {
    unsigned int result = 0;
    for (int i = 0; i <= 31; i++) {
        result |= (x << i);
    }
    return result;
}

/* for loop with compound condition including && */
unsigned int shift_loop_compound(unsigned int x) {
    unsigned int result = 0;
    for (int i = 0; i >= 0 && i < 32; i++) {
        result |= (x << i);
    }
    return result;
}
