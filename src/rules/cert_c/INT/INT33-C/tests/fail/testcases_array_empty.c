/*
 * Rule: INT33-C
 * Source: testcases
 * Status: FAIL - Should trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: FAIL
 * Reason: Division by array size without checking if array is empty (size = 0)
 */

#include <stdio.h>

double calculate_average(int arr[], int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    // No check for size == 0
    return (double)sum / size;  // Divide by zero if size is 0
}

int main() {
    int empty_array[] = {};
    int size = 0;  // Empty array
    double avg = calculate_average(empty_array, size);
    printf("Average: %.2f\n", avg);
    return 0;
}