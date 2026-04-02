/*
 * Rule: POS05-C
 * Source: testcases
 * Status: FAIL - Should trigger POS05-C violation
 *
 * File operation with user input without chroot jail
 */

#include <stdio.h>

int main(int argc, char *argv[]) {
    /* VIOLATION: opening user-supplied path without chroot */
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        fclose(fp);
    }
    return 0;
}
