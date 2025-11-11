/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void process_handler(int sig) {
    // VIOLATION: fork() is not async-safe
    pid_t child_pid = fork();
    if (child_pid == 0) {
        // Child process
        _exit(0);  // _exit is async-safe, but fork is not
    } else if (child_pid > 0) {
        // VIOLATION: wait() and waitpid() are not async-safe
        int status;
        wait(&status);
    }

    // VIOLATION: exec family functions are not async-safe
    if (fork() == 0) {
        execl("/bin/echo", "echo", "Signal handled", NULL);
    }

    // VIOLATION: system() is definitely not async-safe
    system("echo 'Signal received' >> /tmp/signals.log");

    // VIOLATION: getpid() results may be cached and unsafe
    pid_t current_pid = getpid();
}

int main() {
    printf("Demonstrating unsafe process control in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, process_handler);

    printf("Send SIGUSR1 to trigger unsafe process operations\n");

    while (1) {
        pause();
    }

    return 0;
}