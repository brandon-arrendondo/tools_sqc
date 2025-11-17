/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Format string selected from user-controllable array
 */

#include <stdio.h>

int main() {
    char *formats[] = {
        "Format 1: %s\n",
        "Format 2: %d\n",
        "%s"  // This could be manipulated
    };

    int choice;
    char data[] = "test data";

    printf("Enter format choice (0-2): ");
    scanf("%d", &choice);

    if (choice >= 0 && choice < 3) {
        // VULNERABLE: user controls which format string is used
        printf(formats[choice], data);
    }

    return 0;
}