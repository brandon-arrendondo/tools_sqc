/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User-defined template used as format string
 */

#include <stdio.h>

void apply_template(const char *template, const char *data) {
    // VULNERABLE: template parameter used as format string
    printf(template, data);
}

int main() {
    char user_template[100];
    char content[] = "Hello World";

    printf("Enter output template: ");
    fgets(user_template, sizeof(user_template), stdin);

    // VULNERABLE: user input as format template
    apply_template(user_template, content);

    return 0;
}