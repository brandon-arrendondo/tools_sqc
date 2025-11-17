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

volatile sig_atomic_t parent_signals = 0;

void inherited_handler(int sig) {
    parent_signals++;
    printf("Signal %d in process %d\n", parent_signals, getpid());
}

int main() {
    printf("FAIL: Assuming signal handler inheritance behavior\n");

    signal(SIGUSR1, inherited_handler);

    printf("Parent PID: %d\n", getpid());
    printf("Assuming child inherits signal handler\n");

    pid_t pid = fork();
    if (pid == 0) {
        /* Child process */
        printf("Child PID: %d\n", getpid());
        printf("Assuming inherited handler is active\n");

        /* Child assumes it inherited the handler */
        raise(SIGUSR1);
        sleep(1);

        printf("Child signals: %d\n", parent_signals);
        exit(0);
    } else if (pid > 0) {
        /* Parent process */
        sleep(1);

        printf("Sending signal to parent\n");
        raise(SIGUSR1);

        wait(NULL);  /* Wait for child */

        printf("Parent signals: %d\n", parent_signals);
        printf("Handler inheritance behavior varies by platform\n");
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}