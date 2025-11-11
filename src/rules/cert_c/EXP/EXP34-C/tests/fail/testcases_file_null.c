/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using FILE pointer without checking fopen result
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("nonexistent.txt", "r");

    // Using file pointer without checking if fopen succeeded
    char buffer[100];
    fgets(buffer, sizeof(buffer), file);
    printf("Read: %s", buffer);

    fclose(file);
    return 0;
}