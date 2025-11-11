/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Structure member containing user input used as format string
 */

#include <stdio.h>
#include <string.h>

typedef struct {
    char format_template[100];
    char data[50];
} Message;

int main() {
    Message msg;

    printf("Enter format template: ");
    fgets(msg.format_template, sizeof(msg.format_template), stdin);

    strcpy(msg.data, "sample data");

    // VULNERABLE: struct member with user input as format
    printf(msg.format_template, msg.data);

    return 0;
}