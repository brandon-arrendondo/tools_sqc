/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t factory_instances = 0;

void factory_handler(int sig) {
    factory_instances++;
    printf("Factory handler instance %d for signal %d\n", factory_instances, sig);

    // VIOLATION: Handler factory pattern using signal() to create instances
    printf("Creating handler instances with signal() factory pattern\n");

    // Factory creates different handler "instances" for different signals
    switch (factory_instances % 4) {
        case 1:
            printf("Factory: Creating type A handler instances\n");
            if (signal(SIGPIPE, factory_handler) == SIG_ERR) {
                printf("Failed to create type A instance for SIGPIPE\n");
            }
            if (signal(SIGCHLD, factory_handler) == SIG_ERR) {
                printf("Failed to create type A instance for SIGCHLD\n");
            }
            break;

        case 2:
            printf("Factory: Creating type B handler instances\n");
            if (signal(SIGTERM, factory_handler) == SIG_ERR) {
                printf("Failed to create type B instance for SIGTERM\n");
            }
            if (signal(SIGQUIT, factory_handler) == SIG_ERR) {
                printf("Failed to create type B instance for SIGQUIT\n");
            }
            break;

        case 3:
            printf("Factory: Creating type C handler instances\n");
            if (signal(SIGINT, factory_handler) == SIG_ERR) {
                printf("Failed to create type C instance for SIGINT\n");
            }
            if (signal(SIGUSR2, factory_handler) == SIG_ERR) {
                printf("Failed to create type C instance for SIGUSR2\n");
            }
            break;

        case 0:
            printf("Factory: Destroying all instances (reset)\n");
            signal(SIGPIPE, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);
            signal(SIGTERM, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGINT, SIG_DFL);
            signal(SIGUSR2, SIG_DFL);
            break;
    }

    // Factory maintains itself
    if (signal(sig, factory_handler) == SIG_ERR) {
        printf("Factory failed to maintain itself\n");
    }

    printf("Handler factory operation complete (instance %d)\n", factory_instances);
}

int main() {
    printf("SIG34-C VIOLATION: Handler factory pattern using signal() in handlers\n");
    printf("Handler acts as factory creating other handler instances with signal()\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, factory_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to trigger handler factory operations\n");

    while (factory_instances < 8) {
        pause();
    }

    printf("Handler factory instances created: %d\n", factory_instances);
    return 0;
}