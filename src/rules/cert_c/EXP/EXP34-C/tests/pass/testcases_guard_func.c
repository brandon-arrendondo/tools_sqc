/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: Function parameter is validated before use
 */

#include <stdio.h>

void process_data(int *data) {
    if (data == NULL) {
        printf("Error: NULL pointer passed\n");
        return;
    }

    *data = *data * 2;
    printf("Processed value: %d\n", *data);
}

int main() {
    int value = 10;
    process_data(&value);
    process_data(NULL);  // Safe call - function handles NULL
    return 0;
}