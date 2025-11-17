/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Global variable set by user input used as format string
 */

#include <stdio.h>
#include <string.h>

char global_format[100];

void set_format(const char *fmt) {
    strcpy(global_format, fmt);
}

int main() {
    char user_input[100];

    printf("Enter global format: ");
    fgets(user_input, sizeof(user_input), stdin);

    set_format(user_input);

    // VULNERABLE: global variable contains user input
    printf(global_format);

    return 0;
}