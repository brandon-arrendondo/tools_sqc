/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Callback function accesses freed memory passed as parameter
 */

#include <stdlib.h>
#include <stdio.h>

void callback_func(int *data) {
    // BUG: This function assumes data is valid but it might be freed
    printf("Callback received: %d\n", *data);
}

void process_with_callback(int *ptr, void (*cb)(int*)) {
    free(ptr);  // Free the memory
    cb(ptr);    // BUG: Call callback with freed pointer
}

int main() {
    int *data = malloc(sizeof(int));
    if (data == NULL) {
        return -1;
    }

    *data = 999;
    process_with_callback(data, callback_func);

    return 0;
}