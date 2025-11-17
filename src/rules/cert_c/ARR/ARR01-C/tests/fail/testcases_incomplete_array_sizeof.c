/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR01-C violation
 */

#include <stdio.h>

extern int external_array[];

void use_external_array(void) {
    size_t size = sizeof(external_array);

    printf("Size: %zu\n", size);
}

int external_array[10];

int main() {
    use_external_array();
    return 0;
}