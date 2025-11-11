/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: setjmp/longjmp bypasses normal flow, accessing freed memory
 */

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

jmp_buf jump_buffer;
int *global_data;

void cleanup_and_jump() {
    free(global_data);
    longjmp(jump_buffer, 1);
}

int main() {
    global_data = malloc(sizeof(int));
    if (global_data == NULL) {
        return -1;
    }

    *global_data = 444;

    if (setjmp(jump_buffer) == 0) {
        cleanup_and_jump();
    } else {
        // BUG: Jumped here after free, but still access
        printf("After jump: %d\n", *global_data);
    }

    return 0;
}