/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - No violation without call-site data (params assumed non-null)
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Calling function pointer without NULL check
 */

#include <stdio.h>

typedef void (*callback_t)(int);

void process_data(int value, callback_t callback) {
    // No NULL check before calling function pointer
    callback(value);
}

int main() {
    process_data(42, NULL);
    return 0;
}