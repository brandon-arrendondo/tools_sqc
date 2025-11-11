/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Command line argument used as printf format string
 */

#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // VULNERABLE: command line argument as format string
        printf(argv[1]);
        printf("\n");
    }

    return 0;
}