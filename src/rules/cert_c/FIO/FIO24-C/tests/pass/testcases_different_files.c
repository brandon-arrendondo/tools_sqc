/*
 * Rule: FIO24-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO24-C violation
 * Description: Different files opened simultaneously is safe
 */

#include <stdio.h>

void copy_file(void) {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    if (input != NULL && output != NULL) {
        char buf[256];
        while (fgets(buf, sizeof(buf), input) != NULL) {
            fputs(buf, output);
        }
    }

    if (output != NULL) fclose(output);
    if (input != NULL) fclose(input);
}
