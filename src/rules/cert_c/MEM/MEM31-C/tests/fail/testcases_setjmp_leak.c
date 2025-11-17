/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: longjmp bypasses normal cleanup code
 */

#include <stdlib.h>
#include <setjmp.h>

jmp_buf jump_buffer;

void function_with_longjmp() {
    char *buffer = malloc(512);
    if (buffer == NULL) {
        return;
    }

    buffer[0] = 'J';

    if (error_condition()) {
        longjmp(jump_buffer, 1);  // Jumps out without freeing - MEMORY LEAK
    }

    free(buffer);  // This line is never reached
}

void caller() {
    if (setjmp(jump_buffer) == 0) {
        function_with_longjmp();
    } else {
        printf("Jumped back due to error\n");
    }
}

int error_condition() { return 1; }