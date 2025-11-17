/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: Array average calculation checks for empty array before division
 */

#include <stdio.h>

double calculate_average(int arr[], int size) {
    if (size == 0) {
        printf("Error: Cannot calculate average of empty array\n");
        return 0.0;
    }

    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }

    return (double)sum / size;
}

int main() {
    int numbers[] = {10, 20, 30, 40, 50};
    int size = sizeof(numbers) / sizeof(numbers[0]);

    double avg = calculate_average(numbers, size);
    printf("Average: %.2f\n", avg);
    return 0;
}