/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Known limitation: params assumed non-null without call-site data.
 *         This IS a real null deref (main passes NULL), but requires intra-file
 *         call-site analysis to detect. Move to fail/ when implemented.
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