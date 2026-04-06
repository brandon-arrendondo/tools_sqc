/*
 * Rule: INT32-C
 * Source: testcases
 * Status: FAIL - Should trigger INT32-C violation
 */

/*
 * Rule: INT32-C - Ensure that operations on signed integers do not result in overflow
 * Status: FAIL
 * Reason: Unbounded loop counter increment can overflow at INT_MAX
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);

    // VIOLATION: n could be INT_MAX, so n++ overflows
    n++;
    printf("n = %d\n", n);

    return 0;
}