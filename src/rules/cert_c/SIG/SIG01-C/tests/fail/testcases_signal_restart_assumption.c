/*
 * Rule: SIG01-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG01-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

volatile sig_atomic_t restart_count = 0;

void restart_handler(int sig) {
    restart_count++;
    printf("Restart handler called: %d\n", restart_count);
}

int main() {
    printf("FAIL: Assuming signal restart behavior without SA_RESTART\n");

    signal(SIGALRM, restart_handler);

    printf("PID: %d\n", getpid());
    printf("Testing system call restart behavior\n");

    /* Set alarm */
    alarm(2);

    printf("Starting blocking read, alarm will interrupt...\n");

    char buffer[100];
    ssize_t result = read(STDIN_FILENO, buffer, sizeof(buffer));

    if (result == -1) {
        if (errno == EINTR) {
            printf("Read interrupted by signal\n");
        } else {
            perror("read");
        }
    } else {
        printf("Read completed: %zd bytes\n", result);
    }

    printf("Restart count: %d\n", restart_count);
    printf("Assumes consistent restart behavior across platforms\n");

    return 0;
}