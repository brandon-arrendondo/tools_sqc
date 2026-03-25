/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Known limitation: params assumed non-null without call-site data.
 *         This IS a real null deref (main passes NULL), but requires intra-file
 *         call-site analysis to detect. Move to fail/ when implemented.
 */

#include <stdio.h>

void unsafe_function(int *ptr) {
    // No NULL check before dereference
    *ptr = 100;
    printf("Value set to: %d\n", *ptr);
}

int main() {
    unsafe_function(NULL);
    return 0;
}