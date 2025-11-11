/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t handler_active = 0;
volatile sig_atomic_t default_triggered = 0;

void risky_handler(int sig) {
    printf("Custom handler executing for signal %d\n", sig);
    handler_active = 1;

    usleep(1000);

    if (signal(sig, risky_handler) == SIG_ERR) {
        printf("Failed to re-register handler\n");
    } else {
        printf("Handler re-registered\n");
    }

    handler_active = 0;
}

void default_monitor(int sig) {
    printf("DEFAULT HANDLER TRIGGERED! Race condition exposed!\n");
    default_triggered = 1;
    exit(1);
}

int main() {
    printf("Demonstrating race condition in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGTERM, default_monitor);

    if (signal(SIGUSR1, risky_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send rapid SIGUSR1 signals to trigger race condition\n");
    printf("If default handler triggers, race condition is exposed\n");

    for (int i = 0; i < 100; i++) {
        usleep(10000);
        if (default_triggered) {
            break;
        }
    }

    if (!default_triggered) {
        printf("Race condition not triggered in this run\n");
    }

    return 0;
}