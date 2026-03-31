/*
 * Rule: FIO38-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO38-C violation
 * Description: Copying FILE object with memcpy
 */

#include <stdio.h>
#include <string.h>

void copy_stdout(void) {
    FILE backup;
    memcpy(&backup, stdout, sizeof(FILE));  /* Violation: copying FILE */
    fprintf(&backup, "test\n");
}
