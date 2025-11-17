/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: fscanf() return value is not checked for parse errors
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("numbers.txt", "r");
    if (file == NULL) {
        return 1;
    }

    int value;
    // VIOLATION: Return value not checked
    fscanf(file, "%d", &value);

    // Using value assuming parse succeeded
    printf("Value: %d\n", value); // May be uninitialized if parse failed

    // Another unchecked fscanf
    float f_value;
    fscanf(file, "%f", &f_value);
    printf("Float value: %f\n", f_value);

    fclose(file);
    return 0;
}