/*
 * Rule: FIO37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO37-C violation
 * Description: strlen used but not in arithmetic subtraction
 */

#include <stdio.h>
#include <string.h>

void check_length(void) {
    char buf[256];

    if (fgets(buf, sizeof(buf), stdin) != NULL) {
        size_t len = strlen(buf);
        if (len > 0) {
            /* safe to use buf */
        }
    }
}
