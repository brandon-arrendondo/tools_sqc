/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR00-C violation
 */

#include <stdio.h>

void process_fixed_array(int arr[static 10]) {
    for (int i = 0; i < 10; i++) {
        arr[i] = arr[i] * 2;
    }
}

void process_variable_array(int *arr, size_t size) {
    if (arr == NULL || size == 0) {
        return;
    }

    for (size_t i = 0; i < size; i++) {
        arr[i] = arr[i] + 10;
    }
}

double calculate_average(const int arr[], size_t n) {
    if (n == 0) return 0.0;

    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum / n;
}

int main() {
    int numbers[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    process_fixed_array(numbers);

    process_variable_array(numbers, 10);

    double avg = calculate_average(numbers, 10);
    printf("Average: %.2f\n", avg);

    return 0;
}