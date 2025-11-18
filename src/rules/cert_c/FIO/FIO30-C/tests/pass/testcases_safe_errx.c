/*
 * Rule: FIO30-C
 * Source: optional improvements
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule Context: Exclude user input from format strings
 * Safe Case: errx used with literal format string and user input as data argument.
 */

#include <err.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // SAFE: literal format string with user input only as substitution data
        errx(1, "Fatal condition reached for input: %s", argv[1]);
    }
    return 0;
}
