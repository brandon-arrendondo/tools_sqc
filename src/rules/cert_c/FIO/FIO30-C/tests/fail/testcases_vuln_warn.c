/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: User-controlled data (argv) passed directly as format string to warn/errx
 */

#include <err.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // VULNERABLE: argv[1] used as format string
        warn(argv[1]);
        // VULNERABLE: argv[1] used as format string in errx
        errx(1, argv[1]);
    }
    return 0;
}
