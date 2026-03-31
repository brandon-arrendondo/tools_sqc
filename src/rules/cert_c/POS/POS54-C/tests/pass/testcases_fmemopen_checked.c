/*
 * Rule: POS54-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS54-C violation
 *
 * POSIX function return value properly checked
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *in;
    FILE *out;
    size_t size;
    char *ptr;

    in = fmemopen(argv[1], strlen(argv[1]), "r");

    /* COMPLIANT: return value checked for NULL */
    if (in == NULL) {
        return 1;
    }

    out = open_memstream(&ptr, &size);

    if (out == NULL) {
        return 1;
    }

    fclose(in);
    fclose(out);
    return 0;
}
