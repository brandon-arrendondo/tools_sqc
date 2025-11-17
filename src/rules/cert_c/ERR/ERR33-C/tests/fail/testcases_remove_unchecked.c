/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: remove() return value is not checked for file deletion errors
 */

#include <stdio.h>

int main() {
    // VIOLATION: Return value not checked
    remove("nonexistent_file.txt");

    printf("File supposedly removed\n");

    // Another unchecked remove call
    remove("/protected/system/file.txt");
    printf("Protected file supposedly removed\n");

    return 0;
}