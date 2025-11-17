/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR00-C violation
 */

#include <stdio.h>

void update_array(int arr[], int index, int value) {
    arr[index] = value;
}

int main() {
    int numbers[5] = {0};
    int user_index;

    printf("Enter index: ");
    scanf("%d", &user_index);

    update_array(numbers, user_index, 100);

    return 0;
}