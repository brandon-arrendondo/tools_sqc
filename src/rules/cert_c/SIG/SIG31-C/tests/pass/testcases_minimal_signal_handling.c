/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t interrupt_received = 0;

void interrupt_handler(int sig) {
    interrupt_received = 1;
}

int main() {
    printf("Demonstrating minimal safe signal handling\n");
    printf("PID: %d\n", getpid());

    signal(SIGINT, interrupt_handler);

    printf("Press Ctrl+C to interrupt\n");

    struct {
        int counter;
        char message[100];
        double calculations[1000];
    } work_data = {0};

    while (!interrupt_received) {
        work_data.counter++;
        sprintf(work_data.message, "Processing item %d", work_data.counter);

        for (int i = 0; i < 1000; i++) {
            work_data.calculations[i] = (double)i * work_data.counter;
        }

        printf("%s\n", work_data.message);
        sleep(1);

        if (work_data.counter >= 20) {
            printf("Work completed normally\n");
            break;
        }
    }

    if (interrupt_received) {
        printf("\nInterrupt received, cleaning up safely...\n");
        printf("Processed %d items before interruption\n", work_data.counter);
    }

    return 0;
}