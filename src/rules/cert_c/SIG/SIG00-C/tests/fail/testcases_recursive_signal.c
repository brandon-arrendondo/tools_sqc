/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t recursion_depth = 0;

void recursive_handler(int sig) {
    recursion_depth++;
    printf("Recursive handler depth: %d\n", recursion_depth);

    if (recursion_depth < 5) {
        kill(getpid(), SIGUSR1);
        sleep(1);
    }

    recursion_depth--;
    printf("Exiting recursion depth: %d\n", recursion_depth + 1);
}

int main() {
    signal(SIGUSR1, recursive_handler);

    printf("PID: %d\n", getpid());
    printf("Triggering recursive signal handling\n");

    kill(getpid(), SIGUSR1);

    sleep(10);
    printf("Program completed\n");
    return 0;
}