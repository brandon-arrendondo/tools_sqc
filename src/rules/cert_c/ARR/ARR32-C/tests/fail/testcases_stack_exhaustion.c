/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stdio.h>

void exhaust_stack(void) {
    size_t massive_size = 1024 * 1024;  // 1M elements = ~4MB

    int massive_array[massive_size];

    for (size_t i = 0; i < 1000; i++) {
        massive_array[i] = i;
    }

    printf("Created stack-exhausting array\n");
}

int main() {
    exhaust_stack();
    return 0;
}