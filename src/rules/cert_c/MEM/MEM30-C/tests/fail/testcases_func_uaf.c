/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Function frees memory but caller still accesses it afterwards
 */

#include <stdlib.h>
#include <stdio.h>

void process_and_free(int *ptr) {
    if (ptr != NULL) {
        printf("Processing: %d\n", *ptr);
        free(ptr);
    }
}

int main() {
    int *data = malloc(sizeof(int));
    if (data == NULL) {
        return -1;
    }

    *data = 99;
    process_and_free(data);

    // BUG: Access after function freed it
    printf("Value: %d\n", *data);

    return 0;
}