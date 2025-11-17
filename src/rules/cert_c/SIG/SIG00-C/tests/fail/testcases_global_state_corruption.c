/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int value1;
    int value2;
    int checksum;
} shared_data_t;

volatile shared_data_t global_data = {0, 0, 0};

void corrupting_handler(int sig) {
    printf("Handler modifying global data\n");

    // Vulnerable: Non-atomic modification of multi-field structure
    global_data.value1++;
    sleep(1); // Window for interruption
    global_data.value2++;
    global_data.checksum = global_data.value1 + global_data.value2;

    printf("Data modified: v1=%d, v2=%d, checksum=%d\n",
           global_data.value1, global_data.value2, global_data.checksum);
}

int main() {
    struct sigaction sa;

    // Install handler without proper masking
    sa.sa_handler = corrupting_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Not blocking signals during critical section
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send SIGUSR1 and SIGUSR2 quickly to corrupt global state\n");

    while (1) {
        // Check data consistency
        if (global_data.checksum != global_data.value1 + global_data.value2) {
            printf("ERROR: Data corruption detected!\n");
            printf("v1=%d, v2=%d, checksum=%d (should be %d)\n",
                   global_data.value1, global_data.value2, global_data.checksum,
                   global_data.value1 + global_data.value2);
        }
        sleep(1);
    }

    return 0;
}