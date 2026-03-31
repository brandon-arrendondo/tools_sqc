/*
 * Rule: POS05-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS05-C violation
 *
 * File operation with user input inside chroot jail
 */

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    /* COMPLIANT: chroot jail before file access */
    chroot("/var/jail");
    chdir("/");
    setuid(1000);
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        fclose(fp);
    }
    return 0;
}
