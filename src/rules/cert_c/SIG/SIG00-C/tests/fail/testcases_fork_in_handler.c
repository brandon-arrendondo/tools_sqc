/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

volatile sig_atomic_t child_count = 0;

void fork_handler(int sig) {
    printf("Handler: Signal %d received, attempting fork\n", sig);

    // Violation: fork() in signal handler without proper masking
    // can cause process table corruption and resource leaks
    pid_t pid = fork();

    if (pid == -1) {
        perror("Handler: fork failed");
        return;
    } else if (pid == 0) {
        // Child process
        printf("Child: Created from signal handler (signal %d)\n", sig);
        printf("Child: PID = %d, PPID = %d\n", getpid(), getppid());

        // Child does some work then exits
        sleep(2);
        printf("Child: Exiting\n");
        _exit(0);
    } else {
        // Parent process
        child_count++;
        printf("Handler: Created child %d (PID = %d)\n", child_count, pid);

        // Vulnerability: not waiting for child can create zombies
        // especially if handler is interrupted
        sleep(1);
    }
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = fork_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: fork() can be interrupted causing process corruption
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to trigger fork in handler\n");
    printf("This may create zombie processes\n");

    while (1) {
        printf("Main: Running, child count = %d\n", child_count);

        // Occasionally clean up zombies
        int status;
        pid_t zombie;
        while ((zombie = waitpid(-1, &status, WNOHANG)) > 0) {
            printf("Main: Reaped zombie %d\n", zombie);
        }

        sleep(3);
    }

    return 0;
}