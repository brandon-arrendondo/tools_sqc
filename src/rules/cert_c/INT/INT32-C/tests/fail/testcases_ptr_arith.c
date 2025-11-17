/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Pointer arithmetic offset calculation can overflow
 */

#include <limits.h>
#include <stdio.h>

int main() {
    int array[100];
    int base_offset = INT_MAX / 2;
    int additional_offset = INT_MAX / 2;

    // VIOLATION: offset calculation can overflow
    int total_offset = base_offset + additional_offset;

    printf("Total offset: %d\n", total_offset);

    // This would be dangerous pointer arithmetic
    // int* ptr = array + total_offset;

    return 0;
}