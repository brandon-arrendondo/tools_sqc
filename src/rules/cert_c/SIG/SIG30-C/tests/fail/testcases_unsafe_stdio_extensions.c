/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void stdio_handler(int sig) {
    // VIOLATION: Extended stdio functions are not async-safe

    // Stream positioning functions
    FILE *fp = fopen("/tmp/test.txt", "w+");
    if (fp != NULL) {
        // VIOLATION: fseek(), ftell(), rewind() are not async-safe
        fseek(fp, 0, SEEK_END);
        long pos = ftell(fp);
        rewind(fp);

        // VIOLATION: fgetpos(), fsetpos() are not async-safe
        fpos_t file_pos;
        fgetpos(fp, &file_pos);
        fsetpos(fp, &file_pos);

        // VIOLATION: ungetc() is not async-safe
        int ch = fgetc(fp);
        ungetc(ch, fp);

        // VIOLATION: setvbuf() is not async-safe
        setvbuf(fp, NULL, _IONBF, 0);

        // VIOLATION: fileno() may not be async-safe
        int fd = fileno(fp);

        // VIOLATION: fdopen() is not async-safe
        FILE *fp2 = fdopen(fd, "r");
        if (fp2) {
            fclose(fp2);
        }

        fclose(fp);
    }

    // VIOLATION: tmpfile() and tmpnam() are not async-safe
    FILE *temp_fp = tmpfile();
    if (temp_fp) {
        fclose(temp_fp);
    }

    char temp_name[L_tmpnam];
    tmpnam(temp_name);

    // VIOLATION: remove() and rename() are not async-safe
    remove("/tmp/test.txt");
    rename("/tmp/old.txt", "/tmp/new.txt");
}

int main() {
    printf("Demonstrating unsafe stdio extension functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, stdio_handler);

    printf("Send SIGUSR1 to trigger unsafe stdio operations\n");

    while (1) {
        pause();
    }

    return 0;
}