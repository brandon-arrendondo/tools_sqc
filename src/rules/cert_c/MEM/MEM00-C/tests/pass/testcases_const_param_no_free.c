/*
 * Rule: MEM00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM00-C violation
 * Description: Function receives pointer parameter but does not free it
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int validate_data(const char *data, int len) {
    if (data == NULL || len <= 0) return -1;
    if (strlen(data) == 0) return -1;
    return 0;
}

void print_buffer(const char *buf, int size) {
    for (int i = 0; i < size; i++) {
        printf("%c", buf[i]);
    }
    printf("\n");
}
