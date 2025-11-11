/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG35-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

void process_state_handler(int sig) {
    printf("Exception handler: Manipulating process state\n");

    /* Attempt to change process priority */
    if (setpriority(PRIO_PROCESS, 0, 10) == 0) {
        printf("Process priority changed successfully\n");
    } else {
        printf("Failed to change process priority\n");
    }

    /* Get and display process ID information */
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("Process info: PID=%d, PPID=%d\n", pid, ppid);

    /* Attempt to change working directory */
    if (chdir("/tmp/claude") == 0) {
        printf("Working directory changed\n");
    } else {
        printf("Failed to change working directory\n");
    }

    printf("Process state manipulation complete, returning (violates SIG35-C)\n");
    return; /* VIOLATION: returning from computational exception handler */
}

int main() {
    printf("Testing process state manipulation with return\n");
    printf("PID: %d\n", getpid());

    signal(SIGFPE, process_state_handler);

    printf("Triggering floating point exception...\n");
    volatile int zero = 0;
    volatile int result = 1 / zero;

    printf("This represents undefined behavior\n");
    return 0;
}