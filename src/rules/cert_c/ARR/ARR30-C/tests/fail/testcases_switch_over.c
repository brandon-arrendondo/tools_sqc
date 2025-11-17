/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Switch statement case values used as array indices without bounds checking
 */

#include <stdio.h>

int main(void) {
    int lookup[5] = {100, 200, 300, 400, 500};
    int choice = 8;

    switch (choice) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            printf("Valid: lookup[%d] = %d\n", choice, lookup[choice]);
            break;
        default:
            // No bounds checking in default case
            printf("Invalid: lookup[%d] = %d\n", choice, lookup[choice]);
            lookup[choice] = 999;
            break;
    }

    return 0;
}