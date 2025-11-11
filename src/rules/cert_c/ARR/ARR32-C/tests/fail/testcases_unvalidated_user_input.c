/*
 * Rule: ARR32-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stdio.h>

void process_user_array(void) {
    size_t user_size;

    printf("Enter array size: ");
    scanf("%zu", &user_size);

    int array[user_size];

    for (size_t i = 0; i < user_size; i++) {
        array[i] = i;
    }

    printf("Created array of size %zu\n", user_size);
}

int main() {
    process_user_array();
    return 0;
}