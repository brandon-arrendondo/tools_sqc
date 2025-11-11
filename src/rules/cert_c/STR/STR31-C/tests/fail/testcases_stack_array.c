/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Local array too small for string literal assignment
 */

#include <stdio.h>
#include <string.h>

void process_string() {
    char local[8];
    char message[] = "This message is definitely too long";

    strcpy(local, message);  // Message is 35 chars, array only 8
    printf("Local: %s\n", local);
}

int main() {
    process_string();
    return 0;
}