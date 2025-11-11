/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Memory allocated in callback function is never freed
 */

#include <stdlib.h>

typedef void (*callback_t)(void);

char *callback_buffer = NULL;

void allocating_callback() {
    callback_buffer = malloc(256);
    if (callback_buffer != NULL) {
        callback_buffer[0] = 'C';
    }
}

void process_with_callback(callback_t cb) {
    printf("Before callback\n");
    cb();
    printf("After callback\n");
    // callback_buffer is never freed - MEMORY LEAK
}

void main_function() {
    process_with_callback(allocating_callback);
}