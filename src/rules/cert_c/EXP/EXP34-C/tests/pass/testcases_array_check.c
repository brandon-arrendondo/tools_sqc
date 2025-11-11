/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: Array pointer is validated before accessing elements
 */

#include <stdio.h>
#include <stdlib.h>

void print_array(int *arr, size_t size) {
    if (arr == NULL || size == 0) {
        printf("Invalid array or size\n");
        return;
    }

    for (size_t i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int *numbers = malloc(5 * sizeof(int));

    if (numbers != NULL) {
        for (int i = 0; i < 5; i++) {
            numbers[i] = i + 1;
        }
        print_array(numbers, 5);
        free(numbers);
    }

    print_array(NULL, 5);  // Safe - function handles NULL
    return 0;
}