/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Recursive function frees memory but earlier call frame still uses it
 */

#include <stdlib.h>
#include <stdio.h>

int *global_buffer;

void recursive_func(int depth) {
    if (depth == 0) {
        free(global_buffer);
        return;
    }

    printf("Depth %d: %d\n", depth, *global_buffer);
    recursive_func(depth - 1);

    // BUG: Access after deeper recursion freed it
    printf("Returning to depth %d: %d\n", depth, *global_buffer);
}

int main() {
    global_buffer = malloc(sizeof(int));
    if (global_buffer == NULL) {
        return -1;
    }

    *global_buffer = 222;
    recursive_func(3);

    return 0;
}