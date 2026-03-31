/*
 * Rule: FIO37-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO37-C violation
 * Description: strlen used on fgets buffer in expression
 */

#include <stdio.h>
#include <string.h>

void process_input(void) {
    char buf[1024];

    if (fgets(buf, sizeof(buf), stdin) != NULL) {
        int last = strlen(buf) - 1;  /* Violation: underflow if empty */
        if (last >= 0) {
            buf[last] = '\0';
        }
    }
}
