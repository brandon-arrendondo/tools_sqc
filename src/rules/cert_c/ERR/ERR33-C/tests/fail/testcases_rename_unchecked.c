/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: rename() return value is not checked for rename errors
 */

#include <stdio.h>

int main() {
    // VIOLATION: Return value not checked
    rename("old_file.txt", "new_file.txt");

    printf("File supposedly renamed\n");

    // Another unchecked rename
    rename("nonexistent.txt", "also_nonexistent.txt");
    printf("Another rename supposedly completed\n");

    return 0;
}