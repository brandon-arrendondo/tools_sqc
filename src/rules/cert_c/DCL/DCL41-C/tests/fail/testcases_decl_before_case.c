/*
 * Rule: DCL41-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL41-C violation
 * Description: Variable declarations before first case label
 */

#include <stdio.h>

void process(int cmd) {
    switch (cmd) {
        int result;       /* Violation: before first case */
        int status = 0;   /* Violation: before first case */
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
