/*
 * Rule: FIO30-C
 * Source: optional improvements
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Vulnerable Case: GNU error() called with user-controlled argv[1] as format string (index 2)
 */

#include <error.h>

int main(int argc, char *argv[]) {
    if (argc > 2) {
        // VULNERABLE: argv[1] used directly as format string; argv[2] becomes argument data
        error(1, 0, argv[1], argv[2]);
    }
    return 0;
}
