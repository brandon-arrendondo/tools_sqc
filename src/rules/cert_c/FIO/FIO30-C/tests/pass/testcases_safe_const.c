/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Uses const string literals as format strings, not user input
 */

#include <stdio.h>

// Safe: compile-time constant format strings
const char *INFO_FORMAT = "User: %s, Status: %s\n";
const char *ERROR_FORMAT = "Error code: %d, Message: %s\n";

int main() {
    char username[50];
    char status[20];
    int error_code = 404;

    printf("Enter username: ");
    scanf("%49s", username);
    printf("Enter status: ");
    scanf("%19s", status);

    // Safe: using predefined const format strings
    printf(INFO_FORMAT, username, status);
    printf(ERROR_FORMAT, error_code, "File not found");

    return 0;
}