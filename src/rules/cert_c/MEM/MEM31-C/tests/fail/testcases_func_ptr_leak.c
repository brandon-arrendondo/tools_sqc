/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Memory allocated in function called via pointer is never freed
 */

#include <stdlib.h>

typedef void (*operation_func)(void);

char *shared_buffer = NULL;

void allocate_operation() {
    shared_buffer = malloc(512);
    if (shared_buffer != NULL) {
        shared_buffer[0] = 'X';
    }
}

void process_operation() {
    if (shared_buffer != NULL) {
        shared_buffer[1] = 'Y';
    }
}

void execute_operations() {
    operation_func ops[] = {allocate_operation, process_operation};

    for (int i = 0; i < 2; i++) {
        ops[i]();
    }

    // shared_buffer is never freed - MEMORY LEAK
}