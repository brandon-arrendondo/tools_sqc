/*
 * Rule: FIO38-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO38-C violation
 * Description: Copying FILE object with memmove
 */

#include <stdio.h>
#include <string.h>

void move_file_object(FILE *src) {
    FILE copy;
    memmove(&copy, src, sizeof(FILE));  /* Violation: copying FILE */
}
