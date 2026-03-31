/*
 * Rule: EXP34-C
 * Source: testcases (Phase 4 regression)
 * Status: PASS - Global pointer initialized to a string literal (NotNull),
 *         safe to dereference.
 */

#include <stdio.h>
#include <string.h>

const char *global_msg = "hello world";

void print_message(void) {
    printf("Message: %s\n", global_msg);
    printf("Length: %zu\n", strlen(global_msg));
}

int main() {
    print_message();
    return 0;
}
