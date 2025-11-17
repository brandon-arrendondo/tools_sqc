/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>

volatile sig_atomic_t library_count = 0;
jmp_buf jump_buffer;

void library_handler(int sig) {
    library_count++;
    printf("Library signal handler: %d\n", library_count);

    /* Assumes longjmp is safe in signal handler */
    if (library_count >= 2) {
        printf("Jumping out of signal handler\n");
        longjmp(jump_buffer, 1);  /* Unsafe in signal handler */
    }
}

int main() {
    printf("FAIL: Library function usage in signal handler\n");

    signal(SIGINT, library_handler);

    printf("PID: %d - Press Ctrl+C twice\n", getpid());

    if (setjmp(jump_buffer) == 0) {
        printf("Setjmp established, waiting for signals\n");

        /* Loop assumes handler will persist and longjmp will work */
        while (1) {
            pause();
        }
    } else {
        printf("Jumped from signal handler\n");
    }

    printf("Library signals: %d\n", library_count);
    printf("Code uses unsafe library functions in signal handler\n");

    return 0;
}