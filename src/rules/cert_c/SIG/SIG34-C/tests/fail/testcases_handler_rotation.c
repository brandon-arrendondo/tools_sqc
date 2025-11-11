/*
 * Rule: SIG34-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG34-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t rotation_count = 0;

void handler_red(int sig);
void handler_green(int sig);
void handler_blue(int sig);
void handler_yellow(int sig);

void handler_red(int sig) {
    rotation_count++;
    printf("RED handler processing signal %d (rotation %d)\n", sig, rotation_count);

    // VIOLATION: Rotating between different handlers using signal()
    printf("Rotating from RED to GREEN\n");
    if (signal(sig, handler_green) == SIG_ERR) {
        printf("Failed to rotate to GREEN handler\n");
    } else {
        printf("Successfully rotated to GREEN handler\n");
    }
}

void handler_green(int sig) {
    rotation_count++;
    printf("GREEN handler processing signal %d (rotation %d)\n", sig, rotation_count);

    // VIOLATION: Continuing rotation chain with signal()
    printf("Rotating from GREEN to BLUE\n");
    if (signal(sig, handler_blue) == SIG_ERR) {
        printf("Failed to rotate to BLUE handler\n");
    } else {
        printf("Successfully rotated to BLUE handler\n");
    }
}

void handler_blue(int sig) {
    rotation_count++;
    printf("BLUE handler processing signal %d (rotation %d)\n", sig, rotation_count);

    // VIOLATION: Continuing rotation with signal()
    printf("Rotating from BLUE to YELLOW\n");
    if (signal(sig, handler_yellow) == SIG_ERR) {
        printf("Failed to rotate to YELLOW handler\n");
    } else {
        printf("Successfully rotated to YELLOW handler\n");
    }
}

void handler_yellow(int sig) {
    rotation_count++;
    printf("YELLOW handler processing signal %d (rotation %d)\n", sig, rotation_count);

    // VIOLATION: Completing rotation cycle with signal()
    printf("Rotating from YELLOW back to RED\n");
    if (signal(sig, handler_red) == SIG_ERR) {
        printf("Failed to rotate back to RED handler\n");
    } else {
        printf("Successfully rotated back to RED handler\n");
    }
}

int main() {
    printf("SIG34-C VIOLATION: Handlers rotating between different handlers using signal()\n");
    printf("Each handler rotates to the next in sequence: RED->GREEN->BLUE->YELLOW->RED\n");
    printf("PID: %d\n", getpid());

    if (signal(SIGUSR1, handler_red) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to see handler rotation cycle\n");

    while (rotation_count < 16) {
        pause();
    }

    printf("Handler rotations completed: %d\n", rotation_count);
    return 0;
}