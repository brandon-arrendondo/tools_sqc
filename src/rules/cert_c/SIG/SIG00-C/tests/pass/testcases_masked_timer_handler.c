/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

volatile sig_atomic_t timer_ticks = 0;

void safe_timer_handler(int sig) {
    // Compliant: Only atomic operations in signal handler
    timer_ticks++;

    // Using async-safe write instead of printf
    char msg[] = "Timer tick\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main() {
    struct sigaction sa;
    struct itimerval timer;

    sa.sa_handler = safe_timer_handler;
    sigemptyset(&sa.sa_mask);

    // Compliant: Mask SIGALRM during its own handler execution
    // This prevents timer signals from interrupting the handler
    sigaddset(&sa.sa_mask, SIGALRM);

    sa.sa_flags = 0;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Set up repeating timer
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Timer started with proper signal masking\n");

    while (1) {
        printf("Timer ticks received: %d\n", timer_ticks);

        // Complex operations in main thread are safe
        printf("Main thread doing complex work...\n");
        for (int i = 0; i < 3; i++) {
            printf("  Work step %d\n", i + 1);
            sleep(1);
        }
    }

    return 0;
}