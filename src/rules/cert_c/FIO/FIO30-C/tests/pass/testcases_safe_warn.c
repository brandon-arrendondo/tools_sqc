/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: User-controlled data (argv) passed as argument to fixed literal format strings
 */

#include <err.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // SAFE: literal format string with user input as data argument
        warn("User issued command: %s", argv[1]);
        // SAFE: errx with literal format string
        errx(1, "Fatal error encountered: %s", argv[1]);
    }
    return 0;
}
