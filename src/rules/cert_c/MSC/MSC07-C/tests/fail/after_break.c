// Test: code after unconditional break is unreachable
#include <stdio.h>

void search(int *arr, int n, int target) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == target) {
            printf("found at %d\n", i);
            break;
            printf("unreachable\n");  // MSC07-C violation
        }
    }
}
