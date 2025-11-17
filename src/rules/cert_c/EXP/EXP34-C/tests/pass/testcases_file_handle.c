/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: FILE pointer is checked after fopen before use
 */

#include <stdio.h>

int main() {
    FILE *file = fopen("nonexistent.txt", "r");

    if (file == NULL) {
        printf("Failed to open file\n");
        return 1;
    }

    char buffer[100];
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("Read: %s", buffer);
    }

    fclose(file);
    return 0;
}