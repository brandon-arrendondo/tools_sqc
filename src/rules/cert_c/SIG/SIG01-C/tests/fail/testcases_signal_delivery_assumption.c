/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

volatile sig_atomic_t child_signals = 0;

void child_handler(int sig) {
    child_signals++;
    printf("Child signal %d received\n", child_signals);
}

int main() {
    printf("FAIL: Assuming signal delivery behavior across processes\n");

    signal(SIGCHLD, child_handler);

    printf("PID: %d\n", getpid());
    printf("Forking children and assuming SIGCHLD delivery\n");

    /* Fork multiple children assuming signal handler will catch all */
    int i;
    for (i = 0; i < 5; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            /* Child process */
            exit(0);
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    /* Wait for signals, but handler may not persist */
    sleep(2);

    printf("Child signals received: %d (expected 5)\n", child_signals);
    printf("May miss signals if handler resets\n");

    /* Clean up any remaining children */
    while (wait(NULL) > 0);

    return 0;
}