/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: Function pointer is checked before invocation
 */

#include <stdio.h>

typedef void (*callback_t)(int);

void process_with_callback(int value, callback_t callback) {
    if (callback == NULL) {
        printf("No callback provided, using default behavior\n");
        printf("Default: Processing value %d\n", value);
        return;
    }

    callback(value);
}

void my_callback(int value) {
    printf("Callback: Value is %d\n", value);
}

int main() {
    process_with_callback(42, my_callback);
    process_with_callback(24, NULL);  // Safe - function handles NULL
    return 0;
}