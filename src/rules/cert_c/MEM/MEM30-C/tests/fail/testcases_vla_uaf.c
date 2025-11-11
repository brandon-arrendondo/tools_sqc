/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Accessing Variable Length Array after scope ends via saved pointer
 */

#include <stdio.h>

int *saved_ptr;

void create_vla(int size) {
    int vla[size];  // Variable Length Array
    vla[0] = 999;

    // BUG: Save pointer to VLA (stack memory)
    saved_ptr = vla;
}

int main() {
    create_vla(5);

    // BUG: VLA is out of scope, memory may be reused
    printf("VLA value: %d\n", *saved_ptr);

    return 0;
}