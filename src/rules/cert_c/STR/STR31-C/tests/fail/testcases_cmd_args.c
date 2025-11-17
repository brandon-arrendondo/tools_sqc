/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Command line arguments copied to fixed buffer without length checking
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char command_buffer[20];

    if (argc > 1) {
        strcpy(command_buffer, argv[1]);  // argv[1] might be longer than 20 chars
        printf("Command: %s\n", command_buffer);
    }

    return 0;
}