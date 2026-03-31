/*
 * Rule: DCL41-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL41-C violation
 * Description: Declarations properly placed outside switch
 */

#include <stdio.h>

void process(int cmd) {
    int result;
    int status = 0;

    switch (cmd) {
    case 1:
        result = 10;
        printf("%d\n", result);
        break;
    case 2:
        result = 20;
        printf("%d %d\n", result, status);
        break;
    default:
        break;
    }
}
