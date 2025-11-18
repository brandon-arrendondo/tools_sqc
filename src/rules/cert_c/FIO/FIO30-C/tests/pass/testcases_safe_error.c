/*
 * Rule: FIO30-C
 * Source: optional improvements
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Safe Case: GNU error() used with literal format string, user data as substitution.
 */

#include <error.h>

int main(int argc, char *argv[]) {
    if (argc > 2) {
        // SAFE: literal format string; argv[1] & argv[2] used only as data
        error(0, 0, "Processing resource %s with mode %s", argv[1], argv[2]);
    }
    return 0;
}
