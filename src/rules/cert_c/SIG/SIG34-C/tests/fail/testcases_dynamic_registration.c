/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t dynamic_count = 0;

// Array of handler functions for dynamic registration
void handler_alpha(int sig);
void handler_beta(int sig);
void handler_gamma(int sig);

void (*dynamic_handlers[])(int) = {
    handler_alpha,
    handler_beta,
    handler_gamma,
    NULL
};

void handler_alpha(int sig) {
    dynamic_count++;
    printf("Handler Alpha processing signal %d (count: %d)\n", sig, dynamic_count);

    // VIOLATION: Dynamic handler registration using signal() from within handler
    int next_handler = (dynamic_count % 3);
    printf("Dynamically registering handler index %d\n", next_handler);

    if (signal(sig, dynamic_handlers[next_handler]) == SIG_ERR) {
        printf("Failed to dynamically register handler %d\n", next_handler);
    } else {
        printf("Successfully registered dynamic handler %d\n", next_handler);
    }
}

void handler_beta(int sig) {
    dynamic_count++;
    printf("Handler Beta processing signal %d (count: %d)\n", sig, dynamic_count);

    // VIOLATION: Dynamic signal() call based on runtime conditions
    if (dynamic_count > 5) {
        printf("High count: switching to gamma handler\n");
        if (signal(sig, handler_gamma) == SIG_ERR) {
            printf("Failed to switch to gamma handler\n");
        }
    } else {
        printf("Low count: switching to alpha handler\n");
        if (signal(sig, handler_alpha) == SIG_ERR) {
            printf("Failed to switch to alpha handler\n");
        }
    }
}

void handler_gamma(int sig) {
    dynamic_count++;
    printf("Handler Gamma processing signal %d (count: %d)\n", sig, dynamic_count);

    // VIOLATION: Complex dynamic registration logic
    if (dynamic_count % 2 == 0) {
        printf("Even count: registering beta for SIGUSR2\n");
        if (signal(SIGUSR2, handler_beta) == SIG_ERR) {
            printf("Failed to register beta for SIGUSR2\n");
        }
    } else {
        printf("Odd count: registering alpha for current signal\n");
        if (signal(sig, handler_alpha) == SIG_ERR) {
            printf("Failed to register alpha for current signal\n");
        }
    }
}

int main() {
    printf("SIG34-C VIOLATION: Dynamic signal handler registration from within handlers\n");
    printf("Handlers dynamically choose and register other handlers using signal()\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, handler_alpha) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 and SIGUSR2 to see dynamic registration\n");

    while (dynamic_count < 12) {
        pause();
    }

    printf("Dynamic registrations completed: %d\n", dynamic_count);
    return 0;
}